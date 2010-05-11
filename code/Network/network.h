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

#define NETOBJID_MAX UINT32_MAX

using namespace std;

typedef uint32_t netObjID_t;

/* General Networking functions */
int networkInit(); //Initialize the network code.
int setPort(int port);
int setAddr(const char*);
int big_endian();
unsigned long htonf(float f);
float ntohf(unsigned long l);
unsigned long long htond(double d);
double ntohd(unsigned long long a);
#endif
