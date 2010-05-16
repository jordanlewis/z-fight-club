#ifndef PACKET_H
#define PACKET_H

#include <enet/enet.h>
#include <cstring>
#include "Physics/pobject.h"
#include "network.h"

typedef enum {
    RP_START = 0,
    RP_ATTACH_AGENT,
    RP_UPDATE_AGENT,
    RP_KINEMATIC,
    RP_PMOVEABLE,
    RP_CREATE_NET_OBJ,
    RP_ATTACH_PGEOM,
    RP_UPDATE_PGEOM,
    RP_ATTACH_PMOVEABLE,
    RP_UPDATE_PMOVEABLE,
    RP_ATTACH_PAGENT,
    RP_UPDATE_PAGENT,
    RP_PING
} racerPacketType_t;

/* makeRacerPacket:  Creates a RacerPacket.  RacerPackets begin with a
 * racerPacketType_t describing the payload, followed by the payload itself.
 * The packet type will be converted into network byte order before
 * transmission, and so should be passed to makeRacerPacket in host byte order.
 * arg1:  packet type (in host byte order)
 * arg2:  pointer to packet payload (not counting the type)
 * arg3:  size of packet payload (not counting the type)
 */
ENetPacket *makeRacerPacket(racerPacketType_t type, const void *data, int size);

/* getRacerPacketType:  Given a packet, returns the packet type in host byte
 * order */
racerPacketType_t getRacerPacketType(ENetPacket *packet);


struct RPGeomInfo{
    uint32_t type;
    uint32_t radius;
    uint32_t lx, ly, lz;
    uint32_t a, b, c, d;
};

struct RPSteerInfo{
    uint32_t a;
    uint32_t r;
    uint32_t w;
    uint32_t f;
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
    RPSteerInfo info;
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
