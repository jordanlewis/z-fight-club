#include "server.h"
#include "racerpacket.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Agents/agent.h"
#include "Utilities/error.h"
#include "Physics/pobject.h"
#include "racerpacket.h"

Server Server::_instance;

Server::Server() :
    maxConns(DEFAULT_MAX_SERVER_CONNECTIONS),
    pingclock(0),
    world(&World::getInstance()),
    error(&Error::getInstance())
{
    enetAddress.host = htonl(ENET_HOST_ANY);
    enetAddress.port = DEFAULT_NETWORK_PORT;
}

Server::~Server()
{
    if (enetServer != NULL) {
        enet_host_destroy(enetServer);
    }
}

int Server::createNetObj(netObjID_t &ID) {
    int successFlag = 0;
    netObjID_t i = 0;
    //Find smallest unused identifier
    for (; i < NETOBJID_MAX; i++)
    {
        if (netobjs.find(i) == netobjs.end())
        {
            WorldObject *wobject = new WorldObject(NULL, NULL, NULL, NULL);
            netobjs[i] = wobject;
            ID = i;
            successFlag = 1;
            break;
        }
    }

    if (successFlag)
    {
        struct RPCreateNetObj toSend;
        toSend.ID = htonl(i);
        ENetPacket *packet = makeRacerPacket(RP_CREATE_NET_OBJ, &toSend,
                                             sizeof(RPCreateNetObj));
        enet_host_broadcast(enetServer, 0, packet);
    }
    else
    {
        error->log(NETWORK, IMPORTANT, "Cannot accomodate more clients\n");
        return -1;
    }

    return 0;
}

//should return NULL if unable to find an object with the given ID...
WorldObject *Server::getNetObject(netObjID_t ID) {
    return netobjs[ID];
}

int Server::attachPGeom(GeomInfo *info, netObjID_t ID){
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
        {
            cout << "No net object to attach to!" << endl;
            return -1;
        }
    //Attach the PGeom locally
    PGeom *pgeom = new PGeom(info, Physics::getInstance().getOdeSpace());
    obj->pobject = pgeom;
    pgeom->worldObject = obj;

    //Tell networked agents to attach the PGeom
    struct RPAttachPGeom toSend;
    toSend.ID = RP_ATTACH_PGEOM;
    info->hton(&(toSend.info));
    ENetPacket *packet = makeRacerPacket(RP_ATTACH_PGEOM, &toSend,
                                         sizeof(RPAttachPGeom));
    enet_host_broadcast(enetServer, 0, packet);
    return 0;
}

int Server::attachPMoveable(Kinematic *kine, float mass, GeomInfo *info,
                            netObjID_t ID){
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
        {
            cout << "No net object to attach to!" << endl;
            return -1;
        }
    //Attach the PMoveable locally
    PMoveable *pmoveable = new PMoveable(kine, mass, info, 
                                         Physics::getInstance().getOdeSpace());
    obj->pobject = pmoveable;
    pmoveable->worldObject = obj;

    //Tell networked agents to attach the PMoveable
    struct RPAttachPMoveable toSend;
    toSend.ID = RP_ATTACH_PMOVEABLE;
    info->hton(&(toSend.info));
    kine->hton(&(toSend.kine));
    toSend.mass = htonf(mass);
    
    ENetPacket *packet = makeRacerPacket(RP_ATTACH_PMOVEABLE, &toSend,
                                         sizeof(RPAttachPMoveable));
    enet_host_broadcast(enetServer, 0, packet);
    return 0;
}

/* Attaches an agent and corresponding pagent to netObj ID. 
 * Assumes that all agents also want PAgents.  We'll modify this later if
 * that turns out to be a bad assumption.  The mass argument is currently
 * unused, as agents do not currently set their mass.  This can be patched in
 * as needed.
 */
int Server::attachAgent(Kinematic *kine, SteerInfo *steerInfo,
                         float mass, GeomInfo *geomInfo, netObjID_t ID){
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
        {
            cout << "No net object to attach to!" << endl;
            return -1;
        }
    //Attach the Agent locally
    Agent *agent = new Agent();
    agent->setSteering(*steerInfo);
    agent->setKinematic(*kine);

    obj->agent = agent;

    PAgent *pagent = new PAgent(&(agent->getKinematic()),
                                &(agent->getSteering()), mass, geomInfo,
                                Physics::getInstance().getOdeSpace());

    obj->pobject = pagent;
    pagent->worldObject = obj;
    
    //Tell networked agents to attach the PAgent
    struct RPAttachAgent toSend;
    toSend.ID = RP_ATTACH_AGENT;

    geomInfo->hton(&(toSend.info));
    agent->hton(&(toSend.agent));
    
    ENetPacket *packet = makeRacerPacket(RP_ATTACH_AGENT, &toSend,
                                         sizeof(RPAttachAgent));
    enet_host_broadcast(enetServer, 0, packet);
    
    return 0;
}


Server &Server::getInstance()
{
    return _instance;
}

int Server::createHost()
{
    enetServer = enet_host_create(&enetAddress, maxConns, 0, 0);

    if (enetServer == NULL)
        {
            error->log(NETWORK, IMPORTANT, "ENet could not initialize server\n");
            return -1;
        }
    return 0;
}

void Server::setServerAddr(uint32_t addr){
    enetAddress.host = htonl(addr);
    return;
}

void Server::setServerPort(uint16_t port){
    enetAddress.port = port;
    return;
}

void Server::gatherPlayers()
{
    while(1)
    {
        ENetEvent event;
        usleep(10000);
        if (enet_host_service(enetServer, &event, 0) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_NONE:
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                  {
                    error->log(NETWORK, IMPORTANT, "Packet Received\n");
                    racerPacketType_t pt = getRacerPacketType(event.packet);
                    enet_packet_destroy(event.packet);
                    if (pt == RP_START)
                    {
                        netObjID_t netID;
                        if (createNetObj(netID) != 0)
                        {
                            error->log(NETWORK, CRITICAL, "failed to create network object\n");
                        }
                        Agent *agent = world->placeAgent(world->numAgents());
                        BoxInfo box = BoxInfo(agent->width, agent->height, agent->depth);
                        attachAgent(&agent->getKinematic(), &agent->getSteering(), agent->mass, &box, netID);
                        delete agent;
                        return;
                    }
                  }
                    break;
                case ENET_EVENT_TYPE_CONNECT:
                  {
                    error->log(NETWORK, IMPORTANT, "New client connected!");
                    ClientInfo client;
                    int successFlag = 0;
                    client.ipAddr = event.peer->address.host;
                    client.port = event.peer->address.port;

                    //Find smallest unused identifier
                    for (uint8_t i = 0; i < UINT8_MAX; i++){
                        if (clients.find(i) == clients.end()){
                            client.identifier = i;
                            successFlag = 1;
                            break;
                        }
                    }
                    if (successFlag)  {
                        clients[client.identifier] = client;
                    }
                    else {
                        error->log(NETWORK, IMPORTANT, "Cannot accomodate more clients");
                    }
                    break;
                  }
                case ENET_EVENT_TYPE_DISCONNECT:  //NYI
                    error->log(NETWORK, IMPORTANT, "Client disconnecting during startup");
                    break;
            } // end switch
        } // end if
    } // end while
}

/* Packages a netobject's physics data to be sent over the network.  Returns
 * null if a netobject's worldobject has no associated physics pointer.
 */
ENetPacket *Server::packageObject(netObjID_t objID){
    PMoveable *moveable;
    PAgent *agent;
    WorldObject *wobject = getNetObject(objID);

    if (wobject->pobject != NULL)
    {
        //Eeeeewww... dynamic_cast...
        moveable = dynamic_cast<PMoveable *>(wobject->pobject);
        if (moveable != NULL) {
            agent = dynamic_cast<PAgent *>(wobject->pobject);
            if (agent != NULL)
            {
                return makeRacerPacket(RP_UPDATE_PMOVEABLE, moveable,
                                       sizeof(PMoveable));
            }
            else
            {
                return makeRacerPacket(RP_UPDATE_PAGENT, agent,
                                       sizeof(PAgent));
            }
        }
        else
        {
            return makeRacerPacket(RP_UPDATE_PGEOM, wobject->pobject,
                                   sizeof(PGeom));
        }
    }
    return NULL;
}

//General loop structure taken from the tutorial on enet.bespin.org
void Server::serverFrame(){
    error->pin(P_SERVER);
    ENetEvent event;
    usleep(10000);
    racerPacketType_t type;
    void * payload;
    if (pingclock++ == 0)
    {
        // keep the client from disconnecting
        ENetPacket *packet = makeRacerPacket(RP_PING, NULL, 0);
        enet_host_broadcast(enetServer, 0, packet);
    }

    while (enet_host_service(enetServer, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_NONE:
                break;
            case ENET_EVENT_TYPE_CONNECT:
                // no connecting after the game starts.
                // later we might make observers in this case
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                {
                    type = getRacerPacketType(event.packet);
                    payload = event.packet->data+sizeof(racerPacketType_t);
                    switch(type)
                    {
                        case RP_UPDATE_AGENT:
                            {
                                RPUpdateAgent *P = (RPUpdateAgent *)payload;
                                WorldObject *wo = netobjs[P->ID];
                                SteerInfo steerInfo;
                                steerInfo.ntoh(&P->info);
                                cerr << steerInfo << endl;
                                if (wo && wo->agent)
                                {
                                    // do we want to adjust gradally using an average?
                                    wo->agent->setSteering(steerInfo);
                                }
                                break;
                            }
                        default:
                             break;
                    }
                }
                break;
            case ENET_EVENT_TYPE_DISCONNECT:  //NYI
                error->log(NETWORK, IMPORTANT, "Client disconnecting");
                // we should figure out who, and do something about their agent?
                // for now, they'll just slow down and become an obstacle
                // and we'll continue trying to send them updates, unless
                // updates are "multicast" and the disconnect pulls them from that list
                event.peer->data = NULL;
                break;
        }
    }
    error->pout(P_SERVER);
}
