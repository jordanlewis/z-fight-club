#ifndef SERVER_H
#define SERVER_H

#include "network.h"

//Server class
class Server {
    map<uint8_t, ClientInfo> clients; //tracks all connected clients by id
    
    ENetAddress enetAddress;
    ENetHost *enetServer;
  
    Server();
    ~Server();

    static Server _instance;

 public:
    /* Member Functions */
    Server &getInstance();
    
    //Should be called to initialize our server once server info is known:
    int createHost(uint32_t addr, uint16_t port, int maxConns); 
    
    void pushToClient (uint8_t clientId); //NYI
    void updateFromClient(uint8_t clientId); //NYI
    void serverFrame(); //Run the server once through its loop
    int listen(); //Listen for incoming connections.  NYI.
    int closeClient(uint8_t id); //NYI
    
};


#endif
