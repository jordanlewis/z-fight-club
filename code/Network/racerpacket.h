#ifndef PACKET_H
#define PACKET_H

#include <enet/enet.h>
#include <cstring>
#include "allclasses.h"
#include "network.h"
#include "Agents/agent.h"

typedef enum {
    RP_START = 0, //Begin the race
    RP_JOIN, //Add a racer for my agent
    RP_CREATE_AGENT, //Create a net object with an attached agent 
    RP_CREATE_AI_AGENT, //Create a net object with an attached ai agent
    RP_ATTACH_AGENT, //Attach an agent to an existing netobject
    RP_UPDATE_AGENT, //Update an existing agent 
    RP_KINEMATIC, //??????? What is this ????????
    RP_PMOVEABLE, //??????? What is this ????????
    RP_CREATE_NET_OBJ, //Create a new netobject and associated worldobject 
    RP_ATTACH_PGEOM, //Attach a pgeom to an existing netobject
    RP_UPDATE_PGEOM, //Update an existing pgeom
    RP_ATTACH_PMOVEABLE, //Attach a pmoveable to an existing netobject
    RP_UPDATE_PMOVEABLE, //Update an existing pmoveable
    RP_ATTACH_PAGENT, //Attach a pagent to an existing netobject
    RP_UPDATE_PAGENT, //Update an existing pagent
    RP_UPDATE_WEAPONS, //The only way to update weapon state.
    RP_PING, //Keep a connection alive
    RP_RTT, //Determine the RTT for a connection.
    RP_PAUSE,
    RP_UNPAUSE,
    RP_ACK_CONNECTION //Acknowledge successful connection
} racerPacketType_t;

/* makeRacerPacket:  Creates a RacerPacket.  RacerPackets begin with a
 * racerPacketType_t describing the payload, followed by the payload itself.
 * The packet type will be converted into network byte order before
 * transmission, and so should be passed to makeRacerPacket in host byte order.
 * arg1:  packet type (in host byte order)
 * arg2:  pointer to packet payload (not counting the type)
 * arg3:  size of packet payload (not counting the type)
 */
ENetPacket *makeRacerPacket(racerPacketType_t type, const void *data, int size,
                            int flags);

/* getRacerPacketType:  Given a packet, returns the packet type in host byte
 * order */
racerPacketType_t getRacerPacketType(ENetPacket *packet);

struct RPAck{
    uint8_t clientID;
};

struct RPStart{
    uint8_t clientID;
};

struct RPJoin{
    uint8_t clientID;
};

struct RPRTT{
    uint8_t clientID;
    uint64_t time;
};

struct RPPause{
    uint64_t time;
};

struct RPUnpause{
    uint64_t time;
};

struct RPGeomInfo{
    uint32_t type;
    uint32_t radius;
    uint32_t lx, ly, lz;
    uint32_t a, b, c, d;
};

/* Return a pointer to a new geominfo from the parameters passed over the
 * network in netInfo */
GeomInfo *parseRPGeomInfo(RPGeomInfo *netInfo);

struct RPQuat{
    uint32_t x, y, z, w;
};

struct RPPlayerControl {
    uint32_t turnState;
    uint32_t engineState;
    uint32_t weaponState;
};

struct RPUpdateWeapons {
    netObjID_t netID;
    RPPlayerControl control;
    uint16_t ammo[NWEAPONS];
};

struct RPSteerInfo{
    uint32_t a;
    uint32_t r;
    uint32_t w;
    uint32_t f;
};

struct RPCreateAgent {
    netObjID_t netID;
    uint8_t clientID;
};

struct RPCreateAIAgent {
    netObjID_t netID;
};

struct RPKinematic{
    uint32_t posx, posy, posz;
    uint32_t velx, vely, velz;
    uint32_t orienty, orientx, orientz;
    uint32_t orientation;
};

struct RPAgent{
    //uint32_t agentID;
    uint32_t mass, power, maxRotate, height, width, depth;
    RPSteerInfo steerInfo;
    RPKinematic kinematic;
};

struct RPUpdateAgent {
    netObjID_t ID;
    uint8_t AIFlag; //1 if we are an AI
    RPPlayerControl info;
    RPKinematic kine;
    RPQuat quat;
};

struct RPCreateNetObj {
    netObjID_t ID;
};

struct RPAttachPGeom {
    netObjID_t ID;
    RPGeomInfo info;
};

struct RPAttachPMoveable{
    netObjID_t ID;
    RPGeomInfo info;
    RPKinematic kine;
    uint32_t mass;
};

struct RPAttachAgent {
    netObjID_t ID;
    uint8_t clientID;
    RPGeomInfo info;
    RPAgent agent;
};

/*
struct RPAttachPAgent{
    netObjID_t ID;
    RPGeomInfo geomInfo;
    RPKinematic kine;
    uint32_t mass;
    RPSteerInfo steerInfo;
};
*/
#endif
