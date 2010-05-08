#ifndef SERVER_H
#define SERVER_H

#include "network.h"

//Store a client's connection info
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
    int maxConns;
  
    Server();
    ~Server();

    static Server _instance;

 public:
    /* Member Functions */
    static Server &getInstance();
    
    //Should be called to initialize our server once server info is set:
    int createHost(); 
    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    
    void pushToClient (uint8_t clientId); //NYI
    void updateFromClient(uint8_t clientId); //NYI
    void serverFrame(); //Run the server once through its loop
    int listen(); //Listen for incoming connections.  NYI.
    int closeClient(uint8_t id); //NYI
    
};


#endif
