#ifndef SERVER_H
#define SERVER_H

#include "allclasses.h"
#include "network.h"
#include <stdint.h>
#include <list>

//Store a client's connection info
class ClientInfo {
 public:
    uint8_t identifier;  //Unique identifier assigned on connection establish
    uint32_t ipAddr;
    uint16_t port;
    double rtt; //RTT (in seconds) of unreliable packets going to this client
    double updateRate; //Push data to this client at least updateRate ms apart.
    double lastUpdate; //last time we updated this client.
    ENetPeer *peer;
};

//Server class
class Server {
 private:
    map<uint8_t, ClientInfo> clients; //Tracks all connected clients by id
    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects
    list<ENetPacket *> toCreate; //Tracks all objects we need to create

    ENetAddress enetAddress;
    ENetHost *enetServer;
    int maxConns;
    float dt;
    World *world;
    Physics *physics;
    Error *error;
    void initObserver(ENetPeer *);

 public:
    Server();
    ~Server();

    static Server _instance;

    /* attempts to create a netobj.
    * Returns 0 on success, -1 otherwise.
    * Stores the id of the generated netobj
    * in the ID argument. */
    int createNetObj(netObjID_t &ID);

    WorldObject *getNetObject(netObjID_t ID);

    int attachPGeom(GeomInfo *info, netObjID_t ID); 
    int attachPMoveable(Kinematic *kine, float mass, GeomInfo *info,
                        netObjID_t ID);
    
    /* attachAgent:  Assumes all Agents also want a PAgent.  Currently 
     * ignores mass argument.  See server.cpp for more details */ 
    int attachAgent(Kinematic *kine, SteerInfo *steerInfo, 
                    float mass, GeomInfo *geomInfo, netObjID_t ID, 
                    uint8_t clientID); 
    netObjID_t attachNetID(WorldObject *wobject);  /* Associate first
                                                    * available net ID
                                                    * with wobject */
    netObjID_t attachNetID(WorldObject *wobject, netObjID_t ID); /* Associate 
                                                                  * ID with
                                                                  * wobject.*/
    netObjID_t createHumanAgent(uint8_t clientID);
    netObjID_t createAIAgent();
    void createAllAIAgents();
    
    
    void pushAgents(); //Unreliably push non-weapon updates to the client
    void pushWeapons(netObjID_t netID); /* Reliably push an agent's weapon
                                         * updates to all clients */
    void updateAgentsLocally(); //Update agent info from steerinfos
 
    /* Member Functions */
    static Server &getInstance();

    //Should be called to initialize our server once server info is set:
    int createHost();
    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);

    ENetPacket *packageObject(netObjID_t ID);
    void gatherPlayers(); //Service (dis)connections and RP_START requests during setup
    void createAll();   /*Sends out queued up pre-game packets once all clients
                         *are connected */
    int closeClient(uint8_t clientID); //NYI
    void pingClients();
    void checkForPackets();
    void sendPause();
    void sendUnpause();
};


#endif
