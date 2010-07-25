#ifndef CLIENT_H
#define CLIENT_H

#include "allclasses.h"
#include "network.h"
#include "Physics/pweapon.h"
#include "Utilities/vec3f.h"

typedef enum {
    C_CONNECTING = 0,
    C_CONNECTED,
    C_WAITINGFORPLAYER,
    C_PLAYERHASJOINED,
    C_PLAYERREADYTOSTART,
    C_WAITINGFORSTART,
    C_RACE,
    C_PAUSE,
    C_DONE
} clientState_t;

/*! A singleton storing all information related to a networked client.
 */
class Client {
 private:
    uint8_t clientID;  /*!< A network-synchronized client identifier */
    uint32_t clientAddr; /*!< The ipaddr of our client */
    uint16_t clientPort; /*!< The port that our client is running on */
    ENetHost *enetClient; /*!< The ENet library's representation of our client*/

    uint32_t serverAddr; /*!< Address of the server our client connects to */
    uint16_t serverPort; /*!< The port of the server our client connects to */
    double rtt; /*!< Round trip time from client to server */
    double ott; /*!< One way trip time from client to server */
    ENetPeer *peer; /*!< ENet's representation of the server our client connects
                     * to */

    Vec3f pushback; /* If we need to lerp to synch the local player's car with
                     * the server, this tells us in what direction to lerp. */
        
    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects.
    netObjID_t netID; // id for our agent
    PlayerController *player; /* the playercontroller sent across the
                                     * network. This does not directly
                                     * control our player locally. */

    Client();
    ~Client();

    static Client _instance; 
    World *world; /*!< Pointer to the world singleton */
    Input *input; /*!< Pointer to the input singleton */
    Physics *physics; /*!< Pointer to the physics singleton */
    Sound *sound; /*!< Pointer to the sound singleton */
    Error *error; /*!< Pointer to the error singleton */
 public:
    clientState_t clientState; /*!< Has the game started/is it paused/etc. */
    /*  Member Functions  */
    static Client &getInstance(); /*!< Grab the client singleton */

    WorldObject *getNetObj(netObjID_t ID); /*!< Finds the WorldObject associated
                                            * with a netobject */
    /*! Associate netID with a netobject */
    netObjID_t attachNetID(WorldObject *wobject, netObjID_t ID); 

    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);
    int connectToServer();
    void pushToServer();
    void checkForPackets();
    void updateAgentsLocally(); /*!< Update local agents (this is called by
                                 * the scheduler loop). */
    void transmitWeapons(); /*!< Send weapon info to the server */
    void updateDummyController(); /*!< A hack to enable networked weapons. */
    void sendJoinRequest(); /*!< Register yourself as a player in this game. */
    void sendStartRequest(); /*!< Request for the game to start. */
    void sendPause();
    void sendUnpause();
    void sendRTTRequest(); //Determine the RTT for a connection.
    void disconnect();
};

#endif
