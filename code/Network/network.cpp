#include <enet/enet.h>
#include "network.h"

#include <iostream>


using namespace std;


int networkInit()
{
    if (enet_initialize() != 0)
	{
	    cerr << "ENet could not be initialized" << endl;
	    return -1;
	}
    
    return 0;
}


Server::Server(uint32_t addr = ENET_HOST_ANY,
	       uint16_t port = DEFAULT_NETWORK_PORT,
	       int maxConns = DEFAULT_MAX_SERVER_CONNECTIONS)
{
    
    enetAddress.host = addr;
    enetAddress.port = port;
    
    enetServer = enet_host_create(&enetAddress, maxConns, 0, 0);
    
    if (enetServer == NULL)
	{
	    cerr << "ENet could not initialize server" << endl;
	}
}

Server::~Server(){
    enet_host_destroy(enetServer);
}

//General loop structure taken from the tutorial on enet.bespin.org
void Server::serverFrame(){

    ENetEvent event;
    while ( enet_host_service(enetServer, &event, 0) > 0){
	switch (event.type)
	    {
	    case ENET_EVENT_TYPE_NONE:
		break;
	    case ENET_EVENT_TYPE_CONNECT:
		{
		    ClientInfo client;
		    int successFlag = 0;
		    client.ipAddr = event.peer->address.host;
		    client.port = event.peer->address.port;
		
		    //Find smallest unused identifier
		    //for (uint8_t i = UINT8_MIN; i < UINT8_MAX; i++){//Want...
		    for (uint8_t i = 0; i < 255; i++){
			if (clients.find(i) == clients.end()){
			    client.identifier = i;
			    successFlag = 1;
			    break;
			}		    
		    }
		    
		    if (successFlag)  {
			clients[client.identifier] = client;
		    }
		    else {
			cerr << "Cannot accomodate more clients" << endl;
		    }
		    break;
		}
	    case ENET_EVENT_TYPE_RECEIVE: //NYI
		break;

	    case ENET_EVENT_TYPE_DISCONNECT:  //NYI
		break;

	    }
		
    }

}

Client::Client() {
    enetClient = enet_host_create(NULL, 1, 0, 0);

    if (enetClient == NULL)
	{
	    cerr << "Could not initialize client" << endl;
	}
    
}

Client::~Client(){
    enet_host_destroy(enetClient);
}


//Taken mostly from the tutorial at enet.bespin.org
int Client::connectToServer(uint32_t ipAddr, uint16_t port){

    ENetAddress enetAddress;
    ENetEvent event;
    ENetPeer *peer;

    enetAddress.host = ipAddr;
    enetAddress.port = port;
    
    peer = enet_host_connect(enetClient, &enetAddress, 1);

    if (peer == NULL)
	{
	    cerr << "No available peers to connect upon" << endl;
	    return -1;
	}

    //Wait up to 5 seconds to connect
    if (enet_host_service(enetClient, &event, 5000) > 0 &&
	event.type == ENET_EVENT_TYPE_CONNECT)
	{
	    cout << "Connection successful" << endl;
	}
    else 
	{
	    enet_peer_reset(peer);
	    cout << "Connection failed." << endl;
    }

    return 0;

}
