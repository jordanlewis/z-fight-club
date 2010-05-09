#ifndef SERVER_H
#define SERVER_H

#include "network.h"
#include "Engine/world.h"

//Store a client's connection info
class ClientInfo {
 public:
    uint8_t identifier;  //Unique identifier assigned on connection establish
    uint32_t ipAddr;
    uint16_t port;
    double updateRate; //Push data to this client at least updateRate ms apart.
    double lastUpdate; //last time we updated this client.
};

//Server class
class Server {
 private:
    map<uint8_t, ClientInfo> clients; //tracks all connected clients by id
    map<netObjID_t, int> wobjects; /* Tracks all network created world objects.
				  * Assumes at most MAX_INT wobjects created */
    int createNetObj(netObjID_t ID);
    WorldObject *getNetObject(netObjID_t ID);

    ENetAddress enetAddress;
    ENetHost *enetServer;
    int maxConns;
    float dt;

 public:
    Server();
    ~Server();

    static Server _instance;

    void updateFromClient(uint8_t clientId); //NYI (call within serverFrame)

    /* Member Functions */
    static Server &getInstance();
    
    //Should be called to initialize our server once server info is set:
    int createHost(); 
    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    
    void packageObject(netObjID_t ID); //NYI
    void gatherPlayers(); //Service (dis)connections and RP_START requests during setup
    void serverFrame(); //Service incoming packets during gameplay
    int closeClient(uint8_t clientID); //NYI
};


#endif
