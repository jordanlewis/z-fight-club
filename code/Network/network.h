#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <enet/enet.h>

#include "../Engine/world.h"


namespace network{

    //Stores the client's connection info
    class client_info {
        short identifier;  //Unique identifier assigned on connection establish
        char ip_addr[16];
        short port;
    };

    class Client {
        class World net_world; // The latest world recieved on the network

      public:
        class client_info info;
        int last_load;  //The latest image of the world that this client loaded
        int last_rec;  //The latest image of the world that this client recvd.
        class World *the_world; // The world associated with this client
                                // Might be unneccessary.
    
        /*  Member Functions  */
        int connect_to_server(char *addr); //connect to server at addr.
        int disconnect();  //Self-explanatory.
        int get_net_world();  //request a snapshot of the world from the server
        int update_world(); //Copy the contents of net_world into the_world
        };
    
    //Server class
    class Server {
        //vector<class client_info> clients; //will change this soon
        
	ENetAddress enet_address;
	ENetHost *enet_server;

        int world_seq; //Latest world image.
        class World *the_world;  //The definitive image of the world
	
    public:
	/* Member Functions */
	//server();
	Server(unsigned int addr, unsigned int port, int max_conns);
	int push_net_world(short client_id); //sends out a copy of the world
                                             //to client client_id
        int listen(); //Listen for incoming connections
	
	
    };

    /* General Networking functions */

    int network_init(); //Initialize the network code.

}

#endif
