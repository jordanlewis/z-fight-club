#ifndef NETWORK_H
#define NETWORK_H

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <enet/enet.h>
#include <stdint.h> //Pollutes global namespace, but <cstdint> requires c++0x.
#include <map>

#include "Engine/world.h"
#include "constants.h"

//Stores the client's connection info
class ClientInfo {
 public:
    uint8_t identifier;  //Unique identifier assigned on connection establish
    uint32_t ipAddr;
    uint16_t port;
};

//Server class
class Server {
    map<uint8_t, ClientInfo> clients; //tracks all connected clients by id
    
    ENetAddress enetAddress;
    ENetHost *enetServer;
    
 public:
    /* Member Functions */
    Server(uint32_t addr, uint16_t port, int maxConns);
    ~Server();
    void pushToClient (uint8_t clientId); /* sends out a copy of the world
					    to client client_id.  NYI. */
    void updateFromClient(uint8_t clientId);
    void serverFrame(); //Run the server once through its loop
    int listen(); //Listen for incoming connections.  NYI.
    int close_client(uint8_t id); //NYI
    
};

//Client Class
class Client {
 private:
    ClientInfo info;
    ENetHost *enetClient;
   
 public: 
    /*  Member Functions  */
    Client();
    ~Client();
    int connectToServer(uint32_t ipAddr, uint16_t port); //NYI
    void pushToServer(); //NYI
    void updateFromServer();  //NYI
    int disconnect();  //Self-explanatory. NYI
};


/* General Networking functions */

int networkInit(); //Initialize the network code.

#endif
