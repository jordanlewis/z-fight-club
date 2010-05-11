#include "network.h"
#include "Utilities/error.h"
#include "client.h"
#include "server.h"
#include <assert.h>
#include <arpa/inet.h>

int networkInit()
{
    if (enet_initialize() != 0)
	{
	    cerr << "ENet could not be initialized" << endl;
	    return -1;
	}
    
    return 0;
}

int setPort(int port){
    Error &error = Error::getInstance();

    if (port <= 0)
	{
	    error.log(NETWORK, CRITICAL,
		      "port must be a positive integer.");
	    return -1;
	} 
    if (port > 65535) 
	{
	    error.log(NETWORK, CRITICAL,
		      "port must be smaller than 65535");
	    return -1;
	}
    
    switch (World::getInstance().runType)
	{ 
	case CLIENT: 
	    Client::getInstance().setServerPort(port);
	    break;
	case SERVER:
	    Server::getInstance().setServerPort(port);
	    break;
	case SOLO:
	default:
	    error.log(NETWORK, CRITICAL,
		      "Must be in client or server mode to set port.");
	    return -2;
	}	 

    return 0;
}

int setAddr(const char *addr) {
    Error &error = Error::getInstance();
    in_addr address;
    
    //Convert our address into an integer
    if (!inet_aton(addr, &address)) {
	error.log(NETWORK, CRITICAL, 
		  "Invalid ip address");
	return -1;
    }
    
    switch (World::getInstance().runType)
	{ 
	case CLIENT: 
	    Client::getInstance().setServerAddr(address.s_addr);
	    break;
	case SERVER:
	    Server::getInstance().setServerPort(address.s_addr);
	    break;
	case SOLO:
	default:
	    error.log(NETWORK, CRITICAL,
		      "Must be in client or server mode to set address.");
	    return -2;
	}	 

    return 0;
}


int big_endian()
{
    long one= 1;
    return !(*((char *)(&one)));
}

// http://www.gamedev.net/community/forums/topic.asp?topic_id=406142
unsigned long htonf(float f)
{
    assert(sizeof(float) == sizeof(unsigned long));
    return htonl(*reinterpret_cast<long *>(&f));
}

float ntohf(unsigned long l)
{
    assert(sizeof(float) == sizeof(unsigned long));
    unsigned long g = ntohl(l);
    return *reinterpret_cast<float *> (&g);
}

// http://www.dmh2000.com/cpp/dswap.shtml
unsigned long long htond(double d)
{
    assert(sizeof(double) == sizeof(unsigned long long));
    if (big_endian()) return *reinterpret_cast<unsigned long long *> (&d);

    unsigned long long a;
    unsigned char *dst = (unsigned char *)&a;
    unsigned char *src = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return a;
}

double ntohd(unsigned long long a) 
{
    assert(sizeof(double) == sizeof(unsigned long long));
    if (big_endian()) return *reinterpret_cast<double *> (&a);

    double d;
    assert(sizeof(d) == sizeof(a));
    unsigned char *src = (unsigned char *)&a;
    unsigned char *dst = (unsigned char *)&d;

    dst[0] = src[7];
    dst[1] = src[6];
    dst[2] = src[5];
    dst[3] = src[4];
    dst[4] = src[3];
    dst[5] = src[2];
    dst[6] = src[1];
    dst[7] = src[0];

    return d;
}
