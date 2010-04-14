#include <enet/enet.h>
#include "network.h"

#include <iostream>

#define DEFAULT_NETWORK_PORT 6888
#define DEFAULT_MAX_SERVER_CONNECTIONS 16

using namespace std;
using namespace network;

namespace network {
    Server::Server(unsigned int addr = ENET_HOST_ANY,
	   unsigned int port = DEFAULT_NETWORK_PORT,
	   int max_conns = DEFAULT_MAX_SERVER_CONNECTIONS)
    {
	//gethostname()
    
	enet_address.host = addr;
	enet_address.port = port;
	
	enet_server = enet_host_create(&enet_address, max_conns, 0, 0);

	if (enet_server == NULL)
	{
	    cerr << "ENet could not initialize server" << endl;
	}
	
    }
    
    int network_init()
    {
	if (enet_initialize() != 0)
	    {
		cerr << "ENet could not be initialized" << endl;
		return -1;
	    }
	
	return 0;
    }
    
    
}

/*
int main(int argc, const char * argv[]){
    
    network_init();
    
    Server new_server;
    //new_server.init();
    
    Client new_client;
    //new_client.init();

    return 0;
    
}
*/
