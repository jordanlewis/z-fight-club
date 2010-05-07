#include "client.h"

Client Client::_instance;

Client::Client() 
{
    enetClient = enet_host_create(NULL, 1, 0, 0);

    if (enetClient == NULL)
	{
	    cerr << "Could not initialize client" << endl;
	}
    
}

Client::~Client()
{
    enet_host_destroy(enetClient);
}

Client &Client::getInstance()
{
    return _instance;
}

//Taken mostly from the tutorial at enet.bespin.org
int Client::connectToServer(uint32_t ipAddr, uint16_t port)
{
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
