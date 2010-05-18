#ifndef CLIENT_H
#define CLIENT_H

#include "network.h"
#include "Engine/world.h"
class World;
class Input;
class Physics;
class Sound;
class Error;

typedef enum {
    C_CONNECTING = 0,
    C_CONNECTED,
    C_WAITINGFORPLAYER,
    C_PLAYERREADYTOSTART,
    C_WAITINGFORSTART,
    C_RACE,
    C_DONE
} clientState_t;

//Client Class
class Client {
 private:
    uint8_t clientID;
    uint32_t clientAddr;
    uint16_t clientPort;
    ENetHost *enetClient;

    uint32_t serverAddr;
    uint16_t serverPort;
    ENetPeer *peer;

    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects.
    netObjID_t netID; // id for our agent

    Client();
    ~Client();

    static Client _instance;
    World *world;
    Input *input;
    Physics *physics;
    Sound *sound;
    Error *error;
 public:
    clientState_t clientState;
    /*  Member Functions  */
    static Client &getInstance();

    int createNetObj(netObjID_t ID);
    WorldObject *getNetObj(netObjID_t ID);


    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    int connectToServer();
    void pushToServer();
    void checkForPackets();
    void sendStartRequest();
    void disconnect();
};

#endif
