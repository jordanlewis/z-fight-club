#include "client.h"
#include "racerpacket.h"

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
    if (enetClient != NULL) {
	enet_host_destroy(enetClient);
    }
}

Client &Client::getInstance()
{
    return _instance;
}

void Client::setServerAddr(uint32_t addr){
    serverAddr = addr;
    return;
}

void Client::setServerPort(uint16_t port){
    serverPort = port;
    return;
}

//Taken mostly from the tutorial at enet.bespin.org
int Client::connectToServer()
{
    ENetAddress enetAddress;
    ENetEvent event;

    if (serverPort == 0){
	cerr << "No server port specificied" << endl;
	return -1;
    }
    if (serverAddr == 0) {
	cerr << "No server address specified" << endl;
	return -1;
    }

    enetAddress.host = serverAddr;
    enetAddress.port = serverPort;
    
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
	    cout << "Client reports successful connection" << endl;
	}
    else 
	{
	    enet_peer_reset(peer);
	    cout << "Connection failed." << endl;
	    return -1;
    }

    return 0;

}

void Client::sendStartRequest()
{
    ENetPacket *packet = makeRacerPacket(RP_START, NULL, 0);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
}

void Client::pushToServer(){
    string msg = "intentions";
    ENetPacket *packet = makeRacerPacket(RP_AGENT, msg.c_str(), msg.length());
    enet_peer_send(peer, 0, packet);
    enet_host_flush(enetClient);
}
