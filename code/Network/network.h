#ifndef NETWORK_H
#define NETWORK_H

#include <enet/enet.h>
#include <stdint.h> //Pollutes global namespace, but <cstdint> requires c++0x.
#include <map>

#include "Engine/world.h"

//Stores the client's connection info
class ClientInfo {
 public:
    uint8_t identifier;  //Unique identifier assigned on connection establish
    uint32_t ip_addr;
    uint16_t port;
};

//Client Class
class Client {
 private:
    ClientInfo info;
    class World net_world; // The latest world recieved on the network
    int last_load;  //The latest image of the world that this client loaded
    int last_rec;  //The latest image of the world that this client recvd.
    World *the_world; /* The world associated with this client
		      This function might be unneccessary. */
    
 public:
    
    /*  Member Functions  */
    int connect_to_server(ClientInfo* info); //connect to server at addr. NYI
    int disconnect();  //Self-explanatory. NYI
    int get_net_world();  //request a snapshot of the world from the server NYI
    int update_world(); //Copy the contents of net_world into the_world. NYI
};

//Server class
class Server {
    map<short, ClientInfo> clients; //Map tracking all connected clients by id
    
    ENetAddress enet_address;
    ENetHost *enet_server;
    
    int world_seq; //Latest world image.
    class World *the_world;  //The definitive image of the world
	
 public:
    /* Member Functions */
    Server(uint32_t addr, uint16_t port, int max_conns);
    int push_net_world(uint8_t client_id); /* sends out a copy of the world
					    to client client_id.  NYI. */
    int listen(); //Listen for incoming connections.  NYI.
    int close_client(uint8_t id); //NYI
    
};

/* General Networking functions */

int network_init(); //Initialize the network code.

#endif
