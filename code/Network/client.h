#ifndef CLIENT_H
#define CLIENT_H

#include "network.h"

//Client Class
class Client {
 private:
    ClientInfo info;
    ENetHost *enetClient;

    Client();
    ~Client();

    static Client _instance;
 public: 
    /*  Member Functions  */
    Client &getInstance();

    int connectToServer(uint32_t ipAddr, uint16_t port); //NYI
    void pushToServer(); //NYI
    void updateFromServer();  //NYI
    int disconnect();  //Self-explanatory. NYI
};

#endif
