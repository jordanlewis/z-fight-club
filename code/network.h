#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include "world.h"

namespace network{

    //Stores the client's connection info
    class client_info {
        short identifier;  //Unique identifier assigned on connection establish
        char ip_addr[16];
        short port;
    };

    class client {
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
    
        class server {
        //vector<class client_info> clients; //will change this soon
        
        int world_seq; //Latest world image.
        class World *the_world;  //The definitive image of the world
    
        int push_net_world(short client_id); //sends out a copy of the world
                                             //to client client_id
        int listen(); //Listen for incoming connections
    };

}

#endif
