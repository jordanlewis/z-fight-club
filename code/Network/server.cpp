#include "server.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Utilities/error.h"

Server Server::_instance;

Server::Server() 
    : maxConns(DEFAULT_MAX_SERVER_CONNECTIONS)
{
    enetAddress.host = htonl(ENET_HOST_ANY);
    enetAddress.port = DEFAULT_NETWORK_PORT;
}

Server::~Server()
{
    if (enetServer != NULL) {
	enet_host_destroy(enetServer);
    }
}

Server &Server::getInstance()
{
    return _instance;
}

int Server::createHost()
{
    enetServer = enet_host_create(&enetAddress, maxConns, 0, 0);
    
    if (enetServer == NULL)
        {
	    cerr << "ENet could not initialize server" << endl;
	    return -1;
	}
    return 0;
}

void Server::setServerAddr(uint32_t addr){
    enetAddress.host = htonl(addr);
    return;
}

void Server::setServerPort(uint16_t port){
    enetAddress.port = port;
    return;
}

//General loop structure taken from the tutorial on enet.bespin.org
void Server::serverFrame(){

    ENetEvent event;
    usleep(1000000);
    cout << "Server loops" << endl;
    while ( enet_host_service(enetServer, &event, 0) > 0){
	Error error = Error::getInstance();
	switch (event.type)
	    {
	    case ENET_EVENT_TYPE_NONE:
		break;
	    case ENET_EVENT_TYPE_CONNECT:
		{
		    error.log(NETWORK, IMPORTANT, "New client connected!");
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
		    
		    //Place client's agent;
		    Vec3f pos = Vec3f(82,5,28);
		    Agent *agent = new Agent(pos,M_PI/2);
		    World::getInstance().addAgent(agent);

		    break;
		}
	    case ENET_EVENT_TYPE_RECEIVE: //NYI
		error.log(NETWORK, TRIVIAL, "Packet Received");
		break;

	    case ENET_EVENT_TYPE_DISCONNECT:  //NYI
		error.log(NETWORK, IMPORTANT, "Client disconnecting");
		break;

	    }
		
    }

}
