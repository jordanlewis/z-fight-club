#include "compiled_libraries/include/enet/enet.h"
#include "network.h"

#include <iostream>

using namespace std;
using namespace network;

namespace network {
    int server::init()
    {
	//gethostname()
    
	enet_address.host = ENET_HOST_ANY;
	enet_address.port = 6669;
	
	enet_server = enet_host_create(&enet_address, 16, 0, 0);

	if (enet_server == NULL)
	{
	    cerr << "ENet could not initialize server" << endl;
	    return -1;
	}
	
	return 0;
	
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


int main(int argc, const char * argv[]){
    
    network_init();
    
    class server new_server;
    new_server.init();
    
    class client new_client;
    //new_client.init();

    return 0;
    
}

