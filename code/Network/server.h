#ifndef SERVER_H
#define SERVER_H

#include "network.h"
#include "Engine/world.h"

class GeomInfo;
class Kinematic;

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
    map<uint8_t, ClientInfo> clients; //Tracks all connected clients by id
    map<netObjID_t, WorldObject *> netobjs; //Tracks networked world objects

    ENetAddress enetAddress;
    ENetHost *enetServer;
    int maxConns;
    float dt;
    unsigned char pingclock;
    Error *error;

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
                    float mass, GeomInfo *geomInfo, netObjID_t ID); 
    

    void updateFromClient(uint8_t clientId); //NYI (call within serverFrame)

    /* Member Functions */
    static Server &getInstance();

    //Should be called to initialize our server once server info is set:
    int createHost();
    void setServerAddr(uint32_t addr);
    void setServerPort(uint16_t port);

    ENetPacket *packageObject(netObjID_t ID); //NYI
    void gatherPlayers(); //Service (dis)connections and RP_START requests during setup
    void serverFrame(); //Service incoming packets during gameplay
    int closeClient(uint8_t clientID); //NYI
};


#endif
