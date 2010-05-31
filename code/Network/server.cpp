#include "server.h"
#include "racerpacket.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Agents/agent.h"
#include "Agents/player.h"
#include "Agents/ai.h"
#include "Utilities/error.h"
#include "Physics/pobject.h"
#include "Physics/pweapon.h"
#include "racerpacket.h"
#include "Graphics/gobject.h"
#include "Sound/sobject.h"

Server Server::_instance;

Server::Server() :
    maxConns(DEFAULT_MAX_SERVER_CONNECTIONS),
    world(&World::getInstance()),
    physics(&Physics::getInstance()),
    error(&Error::getInstance())
{
    enetAddress.host = htonl(ENET_HOST_ANY);
    enetAddress.port = DEFAULT_NETWORK_PORT;
}

Server::~Server()
{
    if (enetServer != NULL)
    {
        enet_host_destroy(enetServer);
    }
}

/* Creates a world object and attaches a netID to that object.  Tells all 
 * connected clients to also create this object. */
int Server::createNetObj(netObjID_t &ID)
{
    netObjID_t i;
    WorldObject *wobject = new WorldObject(NULL, NULL, NULL, NULL);
    if ((i = attachNetID(wobject)) != NETOBJID_NONE)
    {
        struct RPCreateNetObj toSend;
        toSend.ID = htonl(i);
        ENetPacket *packet = makeRacerPacket(RP_CREATE_NET_OBJ, &toSend,
                                             sizeof(RPCreateNetObj),
                                             ENET_PACKET_FLAG_RELIABLE);
        toCreate.push_back(packet);
    }
    else
    {
        NETWORK << IMPORTANT << "Cannot accomodate more clients" << endl;
        return -1;
    }

    return 0;
}

/* Associate first available net ID with wobject.  Returns the associated ID
 * on success.  Returns NETOBJID_NONE on failure */
netObjID_t Server::attachNetID(WorldObject *wobject)
{
    int successFlag = 0;
    netObjID_t i = 0;
    //Find smallest unused identifier
    for (; i < NETOBJID_MAX; i++)
    {
        if (netobjs.find(i) == netobjs.end())
        {
            netobjs[i] = wobject;
            successFlag = 1;
            break;
        }
    }
    if (!successFlag)
    {
        NETWORK << IMPORTANT << "Cannot accomodate more clients" << endl;
        return NETOBJID_NONE;
    }
    return i;
}

/* Associate ID with wobject */
netObjID_t Server::attachNetID(WorldObject *wobject, netObjID_t ID)
{

    if (NULL != getNetObject(ID))
        {
            error->log(NETWORK, IMPORTANT, "WARNING: Overwrote old netobj\n");
        }

    netobjs[ID] = wobject; 
    return ID;
}

netObjID_t Server::createHumanAgent(uint8_t clientID)
{
    Agent *agent = world->placeAgent(world->numAgents());
    world->addAgent(agent);
    new PlayerController(agent);
    AIManager::getInstance().agentsSorted.push_back(agent);

    netObjID_t netID = attachNetID(agent->worldObject);
    

    if (NETOBJID_NONE == netID)
    {
        error->log(NETWORK, IMPORTANT, "WARNING: ran out of netIDs\n");
        return NETOBJID_NONE;
    } 

    struct RPCreateAgent toSend;
    toSend.netID = htonl(netID);
    toSend.clientID = clientID;
    ENetPacket *packet = makeRacerPacket(RP_CREATE_AGENT, &toSend,
                                         sizeof(RPCreateAgent),
                                         ENET_PACKET_FLAG_RELIABLE);
    toCreate.push_back(packet);

    return netID;
}

netObjID_t Server::createAIAgent()
{
    error->log(NETWORK, TRIVIAL, "Creating an AI Agent\n");
    Agent *agent = world->makeAI();
    netObjID_t netID = attachNetID(agent->worldObject);

    if (NETOBJID_NONE == netID)
    {
        error->log(NETWORK, IMPORTANT, "WARNING: ran out of netIDs\n");
        return NETOBJID_NONE;
    }
    
    struct RPCreateAIAgent toSend;
    toSend.netID = htonl(netID);
    ENetPacket *packet = makeRacerPacket(RP_CREATE_AI_AGENT, &toSend,
                                         sizeof(RPCreateAIAgent),
                                         ENET_PACKET_FLAG_RELIABLE);
    toCreate.push_back(packet);
    
    return netID;
}

void Server::createAllAIAgents()
{
    for (int i = 0; i < world->AIQty; i++)
    {
        createAIAgent();
    }
}


//should return NULL if unable to find an object with the given ID...
WorldObject *Server::getNetObject(netObjID_t ID)
{
    return netobjs[ID];
}

int Server::attachPGeom(GeomInfo *info, netObjID_t ID)
{
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
    {
        error->log(NETWORK, IMPORTANT, "No net object to attach to!\n");
        return -1;
    }
    //Attach the PGeom locally
    PGeom *pgeom = new PGeom(info, Physics::getInstance().getOdeSpace());
    obj->pobject = pgeom;
    pgeom->worldObject = obj;
    obj->gobject = new GObject(info);

    //Tell networked agents to attach the PGeom
    struct RPAttachPGeom toSend;
    toSend.ID = ID;
    info->hton(&(toSend.info));
    ENetPacket *packet = makeRacerPacket(RP_ATTACH_PGEOM, &toSend,
                                         sizeof(RPAttachPGeom),
                                         ENET_PACKET_FLAG_RELIABLE);
    
    toCreate.push_back(packet);
    return 0;
}

int Server::attachPMoveable(Kinematic *kine, float mass, GeomInfo *info,
                            netObjID_t ID)
{
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
    {
        error->log(NETWORK, IMPORTANT, "No net object to attach to!\n");
        return -1;
    }
    //Attach the PMoveable locally
    PMoveable *pmoveable = new PMoveable(kine, mass, info, 
                                         Physics::getInstance().getOdeSpace());
    obj->pobject = pmoveable;
    pmoveable->worldObject = obj;
    obj->gobject = new GObject(info);

    //Tell networked agents to attach the PMoveable
    struct RPAttachPMoveable toSend;
    toSend.ID = ID;
    info->hton(&(toSend.info));
    kine->hton(&(toSend.kine));
    toSend.mass = htonf(mass);
    
    ENetPacket *packet = makeRacerPacket(RP_ATTACH_PMOVEABLE, &toSend,
                                         sizeof(RPAttachPMoveable),
                                         ENET_PACKET_FLAG_RELIABLE);
    
    toCreate.push_back(packet);
    return 0;
}

/* Attaches an agent and corresponding pagent to netObj ID. 
 * Assumes that all agents also want PAgents.  We'll modify this later if
 * that turns out to be a bad assumption.  The mass argument is currently
 * unused, as agents do not currently set their mass.  This can be patched in
 * as needed.
 */
int Server::attachAgent(Kinematic *kine, SteerInfo *steerInfo,
                         float mass, GeomInfo *geomInfo, netObjID_t ID,
                        uint8_t clientID)
{
    WorldObject *obj = getNetObject(ID);
    if (obj == NULL)
    {
        error->log(NETWORK, IMPORTANT, "No net object to attach to!\n");
        return -1;
    }
    //Attach the Agent locally
    Agent *agent = new Agent();
    agent->setSteering(*steerInfo);
    agent->setKinematic(*kine);

    obj->agent = agent;
    agent->worldObject = obj;
    obj->gobject = new GObject(new ObjMeshInfo("ship/"));
    obj->sobject = NULL;
    if (!world->nosound)
    {
        obj->sobject = new SObject("18303_run.wav", GetTime(), AL_TRUE, 0.5);
    }

    PAgent *pagent = new PAgent(&(agent->getKinematic()),
                                &(agent->getSteering()), mass, geomInfo,
                                Physics::getInstance().getOdeSpace());

    pagent->bounce = 1;
    obj->player = new PlayerController(agent);
    obj->pobject = pagent;
    pagent->worldObject = obj;
    world->addObject(obj);    

    //Tell networked agents to attach the PAgent
    struct RPAttachAgent toSend;
    toSend.ID = ID;
    toSend.clientID = clientID;

    geomInfo->hton(&(toSend.info));
    agent->hton(&(toSend.agent));

    ENetPacket *packet = makeRacerPacket(RP_ATTACH_AGENT, &toSend,
                                         sizeof(RPAttachAgent),
                                         ENET_PACKET_FLAG_RELIABLE);
    
    toCreate.push_back(packet);
    return 0;
}

void Server::createAll()
{
    for (list<ENetPacket *>::iterator iter = toCreate.begin();
         iter != toCreate.end();
         iter++)
    {
        enet_host_broadcast(enetServer, 0, *iter);
    }
    return;
}

void Server::pushAgents()
{

    RPUpdateAgent payload;
    for (map<netObjID_t, WorldObject *>::iterator iter = netobjs.begin();
         iter != netobjs.end();
         iter++)
    {
        payload.ID = htonl((*iter).first);
        WorldObject *wo = (*iter).second;
        if (wo == NULL) continue;
        if (wo->player == NULL)  //We are dealing with an AI car.
        {
            payload.AIFlag=1;
        }
        else
        {
            payload.AIFlag = 0;
            wo->player->hton(&(payload.info));
        }
        if (wo->agent == NULL) continue;
        wo->agent->kinematic.hton(&(payload.kine));
        if (wo->pobject == NULL) continue;
        wo->pobject->htonQuat(&(payload.quat));
        ENetPacket *packet = makeRacerPacket(RP_UPDATE_AGENT,
                                             &payload, sizeof(payload),
                                             0);
        enet_host_broadcast(enetServer, 0, packet);
    }
    enet_host_flush(enetServer);
}

void Server::pushWeapons(netObjID_t netID)
{
    WorldObject *wo = netobjs[netID];
    RPUpdateWeapons toSend;
    toSend.netID = htonl(netID);
    if (wo && wo->agent && wo->player)
        {
            wo->player->hton(&(toSend.control));
        }
    else return;
    ENetPacket *packet=makeRacerPacket(RP_UPDATE_WEAPONS, &toSend, 
                                       sizeof(toSend), 
                                       ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(enetServer, 0, packet);
    enet_host_flush(enetServer);
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

void Server::setServerAddr(uint32_t addr)
{
    enetAddress.host = htonl(addr);
    return;
}

void Server::setServerPort(uint16_t port)
{
    enetAddress.port = port;
    return;
}

void Server::checkForPackets()
{
    ENetEvent event;
    racerPacketType_t type;
    void * payload;

    while(enet_host_service(enetServer, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_NONE:
                error->log(NETWORK, TRIVIAL, "EVENT NONE\n");
                assert(0);
                break;
            case ENET_EVENT_TYPE_CONNECT:
              // if race is already started, new client can only be observer
              {
                error->log(NETWORK, TRIVIAL, "EVENT CONNECT\n");
                ClientInfo client;
                int successFlag = 0;
                client.ipAddr = event.peer->address.host;
                client.port = event.peer->address.port;
                client.peer = event.peer;

                //Find smallest unused identifier
                for (uint8_t i = 0; i < UINT8_MAX; i++)
                {
                    if (clients.find(i) == clients.end())
                    {
                        client.identifier = i;
                        successFlag = 1;
                        break;
                    }
                }
                if (successFlag)
                {
                    clients[client.identifier] = client;
                    NETWORK << TRIVIAL << "Sending out client ID #"
                                       << (uint)client.identifier
                                       << endl;
                    struct RPAck toSend;
                    toSend.clientID = client.identifier; // ntonc is trivial
                    ENetPacket *packet = makeRacerPacket(RP_ACK_CONNECTION,
                                                         &toSend,
                                                         sizeof(RPAck),
                                                         ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);
                }
                else
                {
                    NETWORK << IMPORTANT << "Cannot accomodate more clients" << endl;
                }
                break;
              }
            case ENET_EVENT_TYPE_DISCONNECT:
                error->log(NETWORK, TRIVIAL, "EVENT DISCONNECT\n");
                // may need to do something about this...
                event.peer->data = NULL;
                break;
            case ENET_EVENT_TYPE_RECEIVE:
              {
                error->log(NETWORK, TRIVIAL, "EVENT RECEIVE: ");
                type = getRacerPacketType(event.packet);
                NETWORK << TRIVIAL << "Type is: " << type << endl;
                payload = event.packet->data+sizeof(racerPacketType_t);
                switch(type)
                {
                    case RP_JOIN:
                    {
                        NETWORK << TRIVIAL << "RP_JOIN" << endl;
                        RPJoin info = *(RPJoin *)payload;
                        NETWORK << TRIVIAL << "Client # " << info.clientID << " requested join" << endl;
                        if (Scheduler::getInstance().raceState == SETUP)
                        {
                            createHumanAgent(info.clientID);
                        }
                        enet_packet_destroy(event.packet);
                    }
                    case RP_START:
                        error->log(NETWORK, TRIVIAL, "RP_START\n");
                        if (Scheduler::getInstance().raceState != RACE)
                        {
                            RPStart toSend;
                            toSend.clientID = -1; // from the server
                            ENetPacket *packet=makeRacerPacket(RP_START,
                                                               &toSend,
                                                               sizeof(RPStart),
                                                               ENET_PACKET_FLAG_RELIABLE);
                            enet_host_broadcast(enetServer, 0, packet);
                            Scheduler::getInstance().raceState = RACE;
                        }
                        // causes double free: enet_packet_destroy(event.packet);
                        break;
                    case RP_RTT:
                    {
                        error->log(NETWORK, TRIVIAL, "RP_RTT\n");
                        RPRTT info = *(RPRTT *)payload;
                        ENetPacket *packet = makeRacerPacket(RP_RTT,
                                                             &info,
                                                             sizeof(RPRTT),
                                                             0);
                        enet_peer_send(event.peer, 0, packet);
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    case RP_PING:
                        error->log(NETWORK, TRIVIAL, "RP_PING\n");
                        break;
                    case RP_UPDATE_AGENT:
                    {
                        RPUpdateAgent *P = (RPUpdateAgent *)payload;
                        WorldObject *wo = netobjs[ntohl(P->ID)];
                        /*Eeeewwww... we need to ignore weapon updates
                        * here.  It's ugly, but it works, but it's not
                        * like we need to maintain this code beyond
                        * two days from now.
                        */
                        PlayerController netPlayer;
                        netPlayer.ntoh(&(P->info));
                        if (wo && wo->agent && wo->player)
                        {
                            wo->player->setTurnState(netPlayer.getTurnState());
                            wo->player->setEngineState(netPlayer.getEngineState());
                        }
                        break;
                    }
                    case RP_UPDATE_WEAPONS:
                    {
                        NETWORK << TRIVIAL << "Server update weapons!" << endl;
                        RPUpdateWeapons *P = (RPUpdateWeapons *)payload;
                        WorldObject *wo = netobjs[ntohl(P->netID)];
                        PlayerController netPlayer;
                        netPlayer.ntoh(&(P->control));
                        NETWORK << TRIVIAL << netPlayer << endl;
                        if (wo && wo->agent && wo->player)
                        {
                            wo->player->setWeaponState(netPlayer.getWeaponState());
                            pushWeapons(ntohl(P->netID));
                            wo->player->updateAgent();
                            useWeapons(wo->agent);
                            wo->agent->steerInfo.fire = 0;
                        }
                        break;
                    }
                    default:
                        break;
                }
              }
            default: break;
        }
    }
    return;
}

/* Packages a netobject's physics data to be sent over the network.  Returns
 * null if a netobject's worldobject has no associated physics pointer.
 */
ENetPacket *Server::packageObject(netObjID_t objID)
{
    PMoveable *moveable;
    PAgent *agent;
    WorldObject *wobject = getNetObject(objID);

    if (wobject->pobject != NULL)
    {
        //Eeeeewww... dynamic_cast...
        moveable = dynamic_cast<PMoveable *>(wobject->pobject);
        if (moveable != NULL)
        {
            agent = dynamic_cast<PAgent *>(wobject->pobject);
            if (agent != NULL)
            {
                return makeRacerPacket(RP_UPDATE_PMOVEABLE, moveable,
                                       sizeof(PMoveable), 0);
            }
            else
            {
                return makeRacerPacket(RP_UPDATE_PAGENT, agent,
                                       sizeof(PAgent), 0);
            }
        }
        else
        {
            return makeRacerPacket(RP_UPDATE_PGEOM, wobject->pobject,
                                   sizeof(PGeom), 0);
        }
    }
    return NULL;
}

//Updates all agents based on their current steerinfo.
void Server::updateAgentsLocally()
{

    WorldObject *wo = NULL;
    for (map<netObjID_t, WorldObject *>::iterator iter = netobjs.begin();
         iter != netobjs.end();
         iter++)
    {
        wo = iter->second;
        if (wo)
        {
            if (wo->agent && wo->player)
            {
                wo->player->updateAgent();
            }
        }
    }        
}

void Server::pingClients()
{
    // inspired by Engine/component.cpp
    // didn't use it because ping needs a different frequency than Server 
    static double lastRun = 0;
    double time = GetTime();
    if (lastRun + 10 < time)
    {
        lastRun = time;
        ENetPacket *packet = makeRacerPacket(RP_PING, NULL, 0, 0);
        enet_host_broadcast(enetServer, 0, packet);
    }
}
