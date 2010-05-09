#ifndef PACKET_H
#define PACKET_H

#include <enet/enet.h>
#include <cstring>
#include "Physics/pobject.h"
#include "network.h"

typedef enum {
    RP_START = 0,
    RP_AGENT,
    RP_KINEMATIC,
    RP_PMOVEABLE,
    RP_CREATENETOBJ,
    RP_ATTACHKINEMATIC
} racerPacketType_t;

/* makeRacerPacket:  Creates a RacerPacket.  RacerPackets begin with a
 * racerPacketType_t describing the payload, followed by the payload itself.
 * arg1:  packet type
 * arg2:  pointer to packet payload (not counting the type)
 * arg3:  size of packet payload (not counting the type)
 */
ENetPacket *makeRacerPacket(racerPacketType_t type, const void *data, 
			    int size);

struct RPCreateNetObj {
    netObjID_t ID;
};

struct RPAttachPGeom {
    netObjID_t ID;
    PGeom pgeom;
};

struct RPAttachPMoveable{
    netObjID_t ID;
    PMoveable pmoveable;
};

struct RPAttachPAgent{
    netObjID_t ID;
    PAgent pagent;
};

#endif
