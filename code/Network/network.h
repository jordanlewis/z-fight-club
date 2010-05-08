#ifndef NETWORK_H
#define NETWORK_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <enet/enet.h>
#include <stdint.h> //Pollutes global namespace, but <cstdint> requires c++0x.
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Engine/world.h"
#include "constants.h"


using namespace std;

/* General Networking functions */
int networkInit(); //Initialize the network code.
int setPort(int port);
int setAddr(int addr);
#endif
