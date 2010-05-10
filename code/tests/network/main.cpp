// copied from or inspired by http://enet.bespin.org/Tutorial.html

#include <enet/enet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <iostream>
using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define DEFAULT_NETWORK_PORT 6888
#define DEFAULT_MAX_SERVER_CONNECTIONS 16

int parse_server_address(string servername, ENetAddress *serverAddr)
{
    int offset = servername.find(':');
    if ((offset < 1) || ((offset+1) >= (int) servername.length()))
    {
        serverAddr->port = DEFAULT_NETWORK_PORT;
    }
    else
    {
        serverAddr->port = atoi(servername.substr(offset+1).c_str());
    }
    if (enet_address_set_host(serverAddr, servername.substr(0, offset).c_str()) < 0)
    {
        cerr << "couldn't resolve " << servername.substr(0, offset) << endl;
        return -1;
    }
    return 0;
}

string dottedquad(enet_uint32 host)
{
    string a = boost::lexical_cast<string>((host & 0x000000FF) >> 0)
       + "." + boost::lexical_cast<string>((host & 0x0000FF00) >> 8)
       + "." + boost::lexical_cast<string>((host & 0x00FF0000) >> 16)
       + "." + boost::lexical_cast<string>((host & 0xFF000000) >> 24);
    return a;
}

int runServer()
{
    ENetAddress address;
    ENetHost *server;
    address.host = ENET_HOST_ANY;
    address.port = DEFAULT_NETWORK_PORT;

    server = enet_host_create(&address, DEFAULT_MAX_SERVER_CONNECTIONS, 0, 0); // no bandwidth limits
    if (server == NULL)
    {
        cerr << "An error occurred while trying to create an ENet server host." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "waiting for clients to connect" << endl;

    ENetEvent event;
    while (enet_host_service(server, &event, 9000) > 0)
    {
        string s = dottedquad(event.peer->address.host);
        switch (event.type)
        {
          case ENET_EVENT_TYPE_NONE:
            printf("got nothing\n");
            break;
          case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %s:%u.\n", s.c_str(), event.peer->address.port);
            break;
          case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                    (unsigned) event.packet->dataLength,
                    event.packet->data,
                    s.c_str(),
                    event.channelID);
            enet_packet_destroy(event.packet);
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconected.\n", s.c_str());
            event.peer->data = NULL;
        }
    }

    enet_host_destroy(server);
    return 0;
}

int runClient(string servername)
{
    ENetHost *client;
    ENetAddress address;
    ENetPeer *peer;
    ENetEvent event;
    ENetPacket *pkt;

    client = enet_host_create(NULL, 1, 0, 0); // one outgoing connection with no bandwidth limits
    if (client == NULL)
    {
        cerr << "An error occurred while trying to create an ENet client host." << endl;
        exit(EXIT_FAILURE);
    }

    if (parse_server_address(servername, &address) < 0)
    {
        cerr << "failed to parse " << servername << " as host:port" << endl;
        return -1;
    }

    peer = enet_host_connect(client, &address, 2); // allocate two channels
    if (peer == NULL)
    {   
        cerr << "failed to connect to " << servername << endl;
        exit(EXIT_FAILURE);
    }

    if (!(enet_host_service(client, &event, 2000) > 0 &&
         event.type == ENET_EVENT_TYPE_CONNECT))
    {
        cerr << "failed to connect to " << servername << endl;
        exit(EXIT_FAILURE);
    }

    string payload = "hello";
    pkt = enet_packet_create(payload.c_str(), payload.length(), ENET_PACKET_FLAG_RELIABLE);
    if (pkt == NULL)
    {
        cerr << "failed to create hello packet" << endl;
        exit(EXIT_FAILURE);
    }
    if (enet_peer_send(peer, 0, pkt) < 0)
    {
        cerr << "failed to send hello packet" << endl;
        exit(EXIT_FAILURE);
    }
    enet_host_flush(client);

    enet_peer_disconnect(peer, NULL);
    while (enet_host_service(client, &event, 3000) > 0)
    {
        switch (event.type)
        {
          case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
          case ENET_EVENT_TYPE_DISCONNECT:
            cerr << "disconnection succeeded" << endl;
            return 0;
            break;
          default:
            break;
        }
    }
    
    enet_peer_reset(peer);
    enet_host_destroy(client);
    return 0;
}

int main(int argc, char *argv[])
{
    po::variables_map vm;
    string servername;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("server", po::value<string>(), "connect to server, specified as host:port")
        ;

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("server"))
        {
            servername = vm["server"].as<string>();
        }
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
        return EXIT_FAILURE;
    }

    if (enet_initialize() != 0)
    {
        cerr << "An error occurred while initializing ENet." << endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    if (!servername.empty())
    {
        runClient(servername);
    }
    else
    {
        runServer();
    }

    return EXIT_SUCCESS;
}
