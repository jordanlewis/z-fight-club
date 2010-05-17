#include "client.h"
#include "racerpacket.h"
#include <cassert>
#include <boost/lexical_cast.hpp>

Client Client::_instance;

Client::Client() :
    clientID(255),
    world(&World::getInstance()),
    error(&Error::getInstance()),
    clientState(C_NOID)
{
    enetClient = enet_host_create(NULL, 1, 0, 0);

    if (enetClient == NULL)
        {
            cerr << "Could not initialize client" << endl;
        }

}

Client::~Client()
{
    if (enetClient != NULL) {
        enet_host_destroy(enetClient);
    }
}

Client &Client::getInstance()
{
    return _instance;
}

//Should return null if no such element exists in our map.  Does not yet do so.
WorldObject *Client::getNetObj(netObjID_t ID){
    return netobjs[ID];
}

void Client::setServerAddr(uint32_t addr){
    serverAddr = addr;
    return;
}

void Client::setServerPort(uint16_t port){
    serverPort = port;
    return;
}

//Taken mostly from the tutorial at enet.bespin.org
int Client::connectToServer()
{
    error->pin(P_CLIENT);
    ENetAddress enetAddress;
    ENetEvent event;

    if (serverPort == 0){
        cerr << "No server port specificied" << endl;
        return -1;
    }
    if (serverAddr == 0) {
        cerr << "No server address specified" << endl;
        return -1;
    }

    enetAddress.host = serverAddr;
    enetAddress.port = serverPort;

    peer = enet_host_connect(enetClient, &enetAddress, 1);

    if (peer == NULL)
    {
        cerr << "No available peers to connect upon" << endl;
        return -1;
    }

    //Wait up to 5 seconds to connect
    if (enet_host_service(enetClient, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
            cout << "Client reports successful connection" << endl;
    }
    else
    {
            enet_peer_reset(peer);
            cout << "Connection failed." << endl;
            return -1;
    }

    error->pout(P_CLIENT);
    return 0;
}

void Client::checkForStart()
{
    error->pin(P_CLIENT);
    ENetEvent event;
    racerPacketType_t type;
    void * payload;

    while(enet_host_service(enetClient, &event, 100) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            type = getRacerPacketType(event.packet);
            payload = event.packet->data+sizeof(racerPacketType_t);
            if (type == RP_START)
            {
                RPStart info = *(RPStart *)payload;
                clientID = info.clientID;
                cout << "start signal from " << (int) info.clientID << endl;
                clientState = C_START;
                return; // don't process any more packets than I need to.
            } else {
                cerr << "oops, throwing away some packet the client received before it was ready" << endl;
            }
        } else {
            cerr << "oops, some unexpected network event is being ignored by the client" << endl;
        }
    }
    error->pout(P_CLIENT);
}

void Client::checkForAck()
{
    error->pin(P_CLIENT);
    ENetEvent event;
    racerPacketType_t type;
    void * payload;

    while(enet_host_service(enetClient, &event, 100) > 0)
    {
        if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            type = getRacerPacketType(event.packet);
            payload = event.packet->data+sizeof(racerPacketType_t);
            if (type == RP_ACK_CONNECTION)
            {
                // this has my clientID in it, so I'll know when an agent is created just for me
                RPAck info = *(RPAck *)payload;
                clientID = info.clientID;
                cout << "I'm client # " << (unsigned int) clientID << endl;
                clientState = C_HAVEID;
                return; // don't process any more packets than I need to.
            } else {
                cerr << "oops, throwing away some packet the client received before it was ready" << endl;
            }
        } else {
            cerr << "oops, some unexpected network event is being ignored by the client" << endl;
        }
    }
    error->pout(P_CLIENT);
}

void Client::updateFromServer()
{
    error->pin(P_CLIENT);
    ENetEvent event;
    racerPacketType_t type;
    void * payload;

    while(enet_host_service(enetClient, &event, 0) > 0) {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            cerr << "Connection event?  How did that happen?" << endl;
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            type = getRacerPacketType(event.packet);
            payload = event.packet->data+sizeof(racerPacketType_t);
            switch(type)
            {
                case RP_PING:
                    break;
                case RP_ACK_CONNECTION:
                    // this has my clientID in it, so I'll know when an agent is created just for me
                    {
                        RPAck info = *(RPAck *)payload;
                        clientID = info.clientID;
                        cout << "I'm client # " << (unsigned int) clientID << endl;
                    }
                    break;
                case RP_CREATE_NET_OBJ:
                    {
                        RPCreateNetObj info = *(RPCreateNetObj *)payload;
                        WorldObject *wobject = new WorldObject(NULL, NULL, NULL, NULL);
                        netobjs[ntohl(info.ID)] = wobject;
                        string msg = "Created netobj # ";
                        msg += boost::lexical_cast<string>(htonl(info.ID)) + "\n";
                        error->log(NETWORK, TRIVIAL, msg);
                        world->addObject(wobject);
                        break;
                    }
                case RP_ATTACH_PGEOM:
                    {
                        RPAttachPGeom info = *(RPAttachPGeom *)payload;
                        GeomInfo *geomInfo = parseRPGeomInfo(&(info.info));
                        WorldObject *wobject = getNetObj(info.ID);

                        PGeom *geom = new PGeom(geomInfo, Physics::getInstance().getOdeSpace());
                        wobject->pobject = geom;
                        geom->worldObject = wobject;
                    }
                case RP_ATTACH_AGENT: 
                    {
                        RPAttachAgent info = *(RPAttachAgent *)payload;
                        GeomInfo *geomInfo = parseRPGeomInfo(&info.info);
                        WorldObject *wobject = getNetObj(info.ID);

                        Agent *agent = new Agent();
                        agent->ntoh(&(info.agent));
                        wobject->agent = agent;

                        ((BoxInfo*) geomInfo)->lx = 2;
                        ((BoxInfo*) geomInfo)->ly = 2;
                        ((BoxInfo*) geomInfo)->lz = 2;

                        PAgent *pagent = 
                            new PAgent(&(agent->getKinematic()),
                                       &(agent->getSteering()),
                                       agent->mass,
                                       geomInfo, 
                                       Physics::getInstance().getOdeSpace());
                        wobject->pobject = pagent;
                        agent->worldObject = wobject;
                        pagent->worldObject = wobject;
                        wobject->gobject = new GObject(geomInfo);

                        /* if this is *our* agent
                        camera = Camera(THIRDPERSON, agent);
                        Sound::getInstance().registerListener(&camera);
                        PlayerController *p = new PlayerController(agent);
                        Input::getInstance().controlPlayer(p);
                        */

                    }
                default: break;
            }
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            break;
        case ENET_EVENT_TYPE_NONE:
            assert(0);
            break;
        default: break;
        }
    }
    error->pout(P_CLIENT);
}

void Client::sendStartRequest()
{
    error->pin(P_CLIENT);
    RPStart toSend;
    cerr << "about to send start request with clientID=" << (int) clientID << endl;
    toSend.clientID = clientID;
    ENetPacket *packet = makeRacerPacket(RP_START, &toSend, sizeof(RPStart));
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
    error->pout(P_CLIENT);
}

void Client::pushToServer()
{
    error->pin(P_CLIENT);
    /* netobjs is empty right now, use world objects instead
    for (map<netObjID_t, WorldObject *>::iterator iter = netobjs.begin();
         iter != netobjs.end();
         iter++)
    {
        WorldObject *wo = iter->second;
     */
    World &world = World::getInstance();
    for (vector<WorldObject *>::iterator iter = world.wobjects.begin();
         iter != world.wobjects.end();
         iter++)
    {
        WorldObject *wo = *iter;
        if (wo->agent != NULL)
        {
            RPUpdateAgent payload;
            payload.ID = 0xdeadbeef; /* iter->first */
            SteerInfo steerInfo = wo->agent->getSteering();
            steerInfo.hton(&(payload.info));
            ENetPacket *packet = makeRacerPacket(RP_UPDATE_AGENT,
                                                 &payload, sizeof(payload));
            enet_peer_send(peer, 0, packet);
            enet_host_flush(enetClient);
        }
    }
    error->pout(P_CLIENT);
}
