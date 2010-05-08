#include "network.h"
#include "Utilities/error.h"
#include "client.h"
#include "server.h"

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
    World &world = World::getInstance();
    Client &client = Client::getInstance();
    Server &server = Server::getInstance();
    
    if (port < 0)
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
    
    switch (world.runType)
	{ 
	case CLIENT: 
	    client.setServerPort(port);
	    break;
	case SERVER:
	    server.setServerPort(port);
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
    World &world = World::getInstance();
    Client &client = Client::getInstance();
    Server &server = Server::getInstance();
    
    in_addr address;
    
    //Convert our address into an integer
    if (!inet_aton(addr, &address)) {
	error.log(NETWORK, CRITICAL, 
		  "Invalid ip address");
	return -1;
    }
    
    switch (world.runType)
	{ 
	case CLIENT: 
	    client.setServerAddr(address.s_addr);
	    break;
	case SERVER:
	server.setServerPort(address.s_addr);
	break;
	case SOLO:
	default:
	error.log(NETWORK, CRITICAL,
		  "Must be in client or server mode to set address.");
	return -2;
	}	 

    return 0;
}
