#ifndef CLIENT_H
#define CLIENT_H

#include "network.h"

//Client Class
class Client {
 private:
    uint32_t clientAddr;
    uint16_t clientPort;
    ENetHost *enetClient;
    
    uint32_t serverAddr;
    uint16_t serverPort;
    ENetPeer *peer;

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
    int disconnect();  //Self-explanatory. NYI
};

#endif
