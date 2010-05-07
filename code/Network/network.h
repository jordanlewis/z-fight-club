#ifndef NETWORK_H
#define NETWORK_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <enet/enet.h>
#include <stdint.h> //Pollutes global namespace, but <cstdint> requires c++0x.
#include <map>

#include "Engine/world.h"
#include "constants.h"

using namespace std;

//Stores the client's connection info
class ClientInfo {
 public:
    uint8_t identifier;  //Unique identifier assigned on connection establish
    uint32_t ipAddr;
    uint16_t port;
};


/* General Networking functions */
int networkInit(); //Initialize the network code.

#endif
