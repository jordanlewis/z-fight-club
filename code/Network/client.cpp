#include "client.h"
#include "Physics/physics.h"
#include "Physics/pobject.h"
#include "Utilities/error.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Engine/scheduler.h"
#include "Engine/geominfo.h"
#include "Agents/player.h"
#include "Agents/agent.h"
#include "Sound/sound.h"
#include "Graphics/gobject.h"
#include "racerpacket.h"
#include <cassert>
#include <boost/lexical_cast.hpp>

Client Client::_instance;

Client::Client() :
    clientID(255),
    player(NULL),
    world(&World::getInstance()),
    input(&Input::getInstance()),
    physics(&Physics::getInstance()),
    sound(&Sound::getInstance()),
    error(&Error::getInstance()),
    clientState(C_CONNECTING)
{
    enetClient = enet_host_create(NULL, 1, 0, 0);

    if (enetClient == NULL)
        {
            error->log(NETWORK, CRITICAL, "Could not initialize client.\n");
        }
}

Client::~Client()
{
    if (enetClient != NULL)
    {
        enet_host_destroy(enetClient);
    }
}

Client &Client::getInstance()
{
    return _instance;
}

//Should return null if no such element exists in our map.  Does not yet do so.
WorldObject *Client::getNetObj(netObjID_t ID)
{
    return netobjs[ID];
}

/* Associate ID with wobject */
netObjID_t Client::attachNetID(WorldObject *wobject, netObjID_t ID){

    if (NULL != getNetObj(ID))
        {
            error->log(NETWORK, IMPORTANT, "Warning: Overwriting old netobj");
        }

    netobjs[ID] = wobject; 
    return ID;
}

void Client::setServerAddr(uint32_t addr)
{
    serverAddr = addr;
    return;
}

void Client::setServerPort(uint16_t port)
{
    serverPort = port;
    return;
}

//Taken mostly from the tutorial at enet.bespin.org
int Client::connectToServer()
{
    error->pin(P_CLIENT);
    ENetAddress enetAddress;
    ENetEvent event;

    if (serverPort == 0)
    {
        error->log(NETWORK, CRITICAL, "No server port specified.\n");
        return -1;
    }
    if (serverAddr == 0)
    {
        error->log(NETWORK, CRITICAL, "No server address specified.\n");
        return -1;
    }

    enetAddress.host = serverAddr;
    enetAddress.port = serverPort;

    peer = enet_host_connect(enetClient, &enetAddress, 1);

    if (peer == NULL)
    {
        error->log(NETWORK, CRITICAL, "No available peers to connect to.\n");
        return -1;
    }

    //Wait up to 5 seconds to connect
    if (enet_host_service(enetClient, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        error->log(NETWORK, TRIVIAL, "Client reports successful connection.\n");
    }
    else
    {
        enet_peer_reset(peer);
        error->log(NETWORK, CRITICAL, "Connection failed.\n");
        return -1;
    }

    error->pout(P_CLIENT);
    return 0;
}

void Client::checkForPackets()
{
    error->pin(P_CLIENT);
    ENetEvent event;
    racerPacketType_t type;
    void * payload;

    while(enet_host_service(enetClient, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_NONE:
                error->log(NETWORK, TRIVIAL, "EVENT NONE\n");
                assert(0);
                break;
            case ENET_EVENT_TYPE_CONNECT:
                error->log(NETWORK, TRIVIAL, "EVENT CONNECT (to client?!)\n");
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                error->log(NETWORK, TRIVIAL, "EVENT DISCONNECT\n");
                break;
            case ENET_EVENT_TYPE_RECEIVE:
              {
                error->log(NETWORK, TRIVIAL, "EVENT RECEIVE: ");
                type = getRacerPacketType(event.packet);
                payload = event.packet->data+sizeof(racerPacketType_t);
                switch(type)
                {
                    case RP_PING:
                        error->log(NETWORK, TRIVIAL, "RP_PING\n");
                        break;
                    case RP_START:
                      {
                        error->log(NETWORK, TRIVIAL, "RP_START\n");
                        RPStart info = *(RPStart *)payload;
                        clientState = C_RACE;
                        Scheduler::getInstance().raceState = RACE;
                        break;
                      }
                    case RP_ACK_CONNECTION:
                      {
                        error->log(NETWORK, TRIVIAL, "RP_ACK_CONNECTION\n");
                        // this has my clientID in it, so I'll know when an
                        // agent is created just for me
                        RPAck info = *(RPAck *)payload;
                        clientID = info.clientID;
                        string msg = "I'm client # ";
                        msg += boost::lexical_cast<string>((int) clientID) + "\n";
                        error->log(NETWORK, TRIVIAL, msg);
                        clientState = C_CONNECTED;
                        break;
                      }
                    case RP_UPDATE_AGENT: 
                        {
                            
                        error->log(NETWORK, TRIVIAL, "RP_UPDATE_AGENT\n");
                        RPUpdateAgent *P = (RPUpdateAgent *)payload;
                        WorldObject *wo = netobjs[ntohl(P->ID)];
                        if (wo && wo->agent && wo->player && wo->pobject)
                            {
                                wo->player->ntoh(&P->info);
                                /*cout << "PlayerController["
                                     << ntohl(P->ID) << "]: "
                                     << *(wo->player) << endl;*/
                                wo->player->updateAgent();
                                wo->agent->kinematic.ntoh(&(P->kine));
                                wo->pobject->ntohQuat(&(P->quat));
                            }
                            
                        break;
                        }
                    case RP_CREATE_NET_OBJ:
                      {
                        error->log(NETWORK, TRIVIAL, "RP_CREATE_NET_OBJ\n");
                        RPCreateNetObj info = *(RPCreateNetObj *)payload;
                        WorldObject *wobject = new WorldObject(NULL, NULL, NULL, NULL);
                        attachNetID(wobject, ntohl(info.ID));
                        string msg = "Created netobj # ";
                        msg += boost::lexical_cast<string>(htonl(info.ID)) + "\n";
                        error->log(NETWORK, TRIVIAL, msg);
                        world->addObject(wobject);
                        break;
                      }
                    case RP_ATTACH_PGEOM:
                      {
                        error->log(NETWORK, TRIVIAL, "RP_ATTACH_PGEOM\n");
                        RPAttachPGeom info = *(RPAttachPGeom *)payload;
                        GeomInfo *geomInfo = parseRPGeomInfo(&(info.info));
                        WorldObject *wobject = getNetObj(info.ID);
                        PGeom *geom = new PGeom(geomInfo, physics->getOdeSpace());
                        wobject->pobject = geom;
                        geom->worldObject = wobject;
                      }
                    case RP_ATTACH_AGENT:
                      {
                        error->log(NETWORK, TRIVIAL, "RP_ATTACH_AGENT\n");
                        RPAttachAgent info = *(RPAttachAgent *)payload;
                        GeomInfo *geomInfo = parseRPGeomInfo(&info.info);
                        WorldObject *wobject = getNetObj(info.ID);
                        Agent *agent = new Agent();
                        agent->ntoh(&(info.agent));
                        wobject->agent = agent;

                        ((BoxInfo*) geomInfo)->lx = agent->width;
                        ((BoxInfo*) geomInfo)->ly = agent->height;
                        ((BoxInfo*) geomInfo)->lz = agent->depth;

                        PAgent *pagent =
                            new PAgent(&(agent->getKinematic()),
                                       &(agent->getSteering()),
                                       agent->mass,
                                       geomInfo,
                                       physics->getOdeSpace());
                        pagent->bounce = 1;
                        wobject->pobject = pagent;
                        agent->worldObject = wobject;
                        pagent->worldObject = wobject;
                        wobject->gobject = new GObject(new ObjMeshInfo("ship/"));
                        wobject->player = new PlayerController(agent);

                        if (info.clientID == clientID)
                        {
                            error->log(NETWORK, TRIVIAL, " my agent\n");
                            netID = info.ID;
                            world->camera = Camera(THIRDPERSON, agent);
                            sound->registerListener(&world->camera);
                            player = &input->getPlayerController();
                        }
                        else
                        {
                            error->log(NETWORK, TRIVIAL, " not my agent\n");
                        }
                      }
                    default:
                        break;
                }
              }
            default: break;
        }
    }
    
    //Updates all agents based on their current steerinfo.  Should be
    //factored out into another function, prehaps?

    error->pout(P_CLIENT);
    return;
}

void Client::updateAgentsLocally(){
    error->pin(P_CLIENT);

    WorldObject *wo = NULL;
    for (map<netObjID_t, WorldObject *>::iterator iter = netobjs.begin();
         iter != netobjs.end();
         iter++){
        wo = iter->second;
        if (wo) {
            if (wo->agent && wo->player){
                wo->player->updateAgent();
            }
        }
    }        
    
    error->pout(P_CLIENT);
}

void Client::sendJoinRequest(){
    error->pin(P_CLIENT);
    RPJoin toSend;
    toSend.clientID = clientID;
    ENetPacket *packet = makeRacerPacket(RP_JOIN, &toSend, sizeof(RPJoin),
                                         ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
    error->pout(P_CLIENT);
}

void Client::sendStartRequest()
{
    error->pin(P_CLIENT);
    RPStart toSend;
    toSend.clientID = clientID;
    ENetPacket *packet = makeRacerPacket(RP_START, &toSend, sizeof(RPStart),
                                         ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
    error->pout(P_CLIENT);
}

void Client::pushToServer()
{
    error->pin(P_CLIENT);
    RPUpdateAgent payload;
    payload.ID = htonl(netID);
    if (player == NULL) return;
    player->hton(&(payload.info));
    ENetPacket *packet = makeRacerPacket(RP_UPDATE_AGENT,
                                         &payload, sizeof(payload),
                                         0);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
    error->pout(P_CLIENT);
}

void Client::disconnect()
{
    ENetEvent event;
    enet_peer_disconnect (peer, 0);
    while (enet_host_service (enetClient, &event, 3000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            error->log(NETWORK, TRIVIAL, "Disconnection succeeded.\n");
            return;
        default:
            break;
        }
    }
    enet_peer_reset(peer);
}
