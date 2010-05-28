#ifndef CLIENT_H
#define CLIENT_H

#include "allclasses.h"
#include "network.h"
#include "Utilities/vec3f.h"

#define NET_RTT_MIX_FACTOR .8 //newRTT=mix_factor*oldRTT+(1-mix_factor)freshRTT
#define NET_RANGE_FUDGE 5 /* Allow a fudge factor in our calculations of 
                             * acceptable error in the client's dead reckoning.
                             * This helps account for acceleration, as the 
                             * acceptable error only takes velocity into
                             * account. */

typedef enum {
    C_CONNECTING = 0,
    C_CONNECTED,
    C_WAITINGFORPLAYER,
    C_PLAYERHASJOINED,
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
    double rtt; //Round trip time
    double ott; //One way trip time
    ENetPeer *peer;

    Vec3f pushback; /* If we need to lerp to synch to the server, this tells
                     * us what direction to lerp in. */
        
    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects.
    netObjID_t netID; // id for our agent
    PlayerController *player; /* the playercontroller sent across the
                                     * network. This does not directly
                                     * control our player locally. */

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
    netObjID_t attachNetID(WorldObject *wobject, netObjID_t ID);

    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    int connectToServer();
    void pushToServer();
    void checkForPackets();
    void updateAgentsLocally();
    void sendJoinRequest(); //Register yourself as a player in this game.
    void sendStartRequest(); //Request for the game to start.
    void sendRTTRequest(); //Determine the RTT for a connection.
    void disconnect();
};

#endif
