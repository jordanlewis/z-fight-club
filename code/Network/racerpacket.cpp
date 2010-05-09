#include "racerpacket.h"

ENetPacket *makeRacerPacket(racerPacketType_t type, const void *data, int size){

    ENetPacket *packet = enet_packet_create(&type,
					    sizeof(racerPacketType_t) + size,
					    0);
    memcpy(&(packet->data[sizeof(racerPacketType_t)]), data, size);
    return packet;
}

