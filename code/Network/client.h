#ifndef CLIENT_H
#define CLIENT_H

#include "network.h"
#include "Engine/world.h"

//Client Class
class Client {
 private:
    uint32_t clientAddr;
    uint16_t clientPort;
    ENetHost *enetClient;
    
    uint32_t serverAddr;
    uint16_t serverPort;
    ENetPeer *peer;

    map<netObjID_t, int> wobjects; /* Tracks all network created world objects.
				  * Assumes at most MAX_INT wobjects created */
    int createNetObj(netObjID_t ID);
    WorldObject *getNetObj(netObjID_t ID);

    Client();
    ~Client();

    static Client _instance;
 public: 
    /*  Member Functions  */
    static Client &getInstance();

    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    int connectToServer();
    void pushToServer(); //NYI
    void updateFromServer();  //NYI
    void sendStartRequest();
    int disconnect();  //Self-explanatory. NYI
};

#endif
