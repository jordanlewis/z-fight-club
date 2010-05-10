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

    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects.
    
    Client();
    ~Client();

    static Client _instance;
 public: 
    /*  Member Functions  */
    static Client &getInstance();

    int createNetObj(netObjID_t ID);
    WorldObject *getNetObj(netObjID_t ID);


    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    int connectToServer();
    void pushToServer(); //NYI
    void updateFromServer();  //NYI
    void sendStartRequest();
    int disconnect();  //Self-explanatory. NYI
};

#endif
