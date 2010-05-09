#ifndef PACKET_H
#define PACKET_H

#include <enet/enet.h>
#include <cstring>


typedef enum {
    RP_START = 0,
    RP_AGENT,
    RP_KINEMATIC,
    RP_PMOVEABLE
} racerPacketType_t;

/* makeRacerPacket:  Creates a RacerPacket.  RacerPackets begin with a
 * racerPacketType_t describing the payload, followed by the payload itself.
 * arg1:  packet type
 * arg2:  pointer to packet payload (not counting the type)
 * arg3:  size of packet payload (not counting the type)
 */
ENetPacket *makeRacerPacket(racerPacketType_t type, void *data, int size);

#endif
