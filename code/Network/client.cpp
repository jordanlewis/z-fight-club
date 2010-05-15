#include "client.h"
#include "racerpacket.h"
#include <cassert>
#include <boost/lexical_cast.hpp>

Client Client::_instance;

Client::Client()
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

    return 0;

}

void Client::updateFromServer()
{
    Error& error = Error::getInstance();
    error.pin(P_CLIENT);
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
                case RP_CREATE_NET_OBJ:
                    {
                        RPCreateNetObj info = *(RPCreateNetObj *)payload;
                        WorldObject *wobject = new WorldObject(NULL, NULL, NULL, NULL);
                        netobjs[ntohl(info.ID)] = wobject;
                        string msg = "Created netobj # ";
                        msg += boost::lexical_cast<string>(htonl(info.ID)) + "\n";
                        error.log(NETWORK, TRIVIAL, msg);

                        break;
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
    error.pout(P_CLIENT);
}

void Client::sendStartRequest()
{
    ENetPacket *packet = makeRacerPacket(RP_START, NULL, 0);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
}

void Client::pushToServer(){
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
}
