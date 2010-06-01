#ifndef NETWORK_H
#define NETWORK_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <enet/enet.h>
#include <stdint.h> //Pollutes global namespace, but <cstdint> requires c++0x.
#include <limits>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "allclasses.h"
#include "constants.h"

//#define NETOBJID_MAX (UINT32_MAX - 1)
//#define NETOBJID_NONE UINT32_MAX

#define NETOBJID_MAX (4294967295U - 1)
#define NETOBJID_NONE (NETOBJID_MAX + 1)

using namespace std;

typedef uint32_t netObjID_t;

/* General Networking functions */
int networkInit(); //Initialize the network code.
int setPort(int port);
int setAddr(const char*);
int big_endian();
uint32_t htonf(float f);
float ntohf(uint32_t i);
uint64_t htond(double d);
double ntohd(uint64_t i);
#endif
