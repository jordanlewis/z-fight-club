/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "math.h"
#include "scheduler.h"
#include "Physics/physics.h"
#include "Graphics/graphics.h"
#include "Sound/sound.h"
#include "Network/network.h"
#include "Network/client.h"
#include "Network/server.h"
#include "input.h"
#include "setup.h"

#include "Utilities/vec3f.h"

using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::variables_map vm;
    World &world = World::getInstance();
    Sound     &sound    = Sound::getInstance();
    Graphics  &graphics = Graphics::getInstance();
    Scheduler &scheduler = Scheduler::getInstance();
    Client &client = Client::getInstance();
    Server &server = Server::getInstance();

    try {
        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("track", po::value<string>(), "set track file")
            ("assets", po::value<string>(), "set assets directory")
            ("network", po::value<string>(), "set network mode")
            ("ipaddr", po::value<string>(), "set server ip address")
            ("port", po::value<int>(), "set server port")
            ("ai-players", po::value<int>(), "set number of AI players")
            ("nox", "disable graphics")
            ("nosound", "disable sound")
        ;

        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("track"))
        {
            world.loadTrack(vm["track"].as<string>().c_str());
        }
        else
        {
            cout << "Using default track tests/tracks/oval.trk" << endl;
            world.loadTrack("tests/tracks/oval.trk");
        }

        if (vm.count("assets"))
        {
            world.setDir(vm["assets"].as<string>().c_str());
        }
        else
        {
            cout << "Using default assets dir ../assets/" << endl;
            world.setDir("../assets/");
        }

        if (vm.count("network"))
        {
            world.setRunType(vm["network"].as<string>().c_str());
        }
        else
        {
            world.setRunType("Solo");
        }
        if (!vm.count("no-human"))
        {
            world.PlayerQty = 1;
        }
        if (vm.count("ai-players") && (world.runType == SOLO || world.runType == CLIENT))
        {
            world.AIQty = vm["ai-players"].as<int>();
        }
        else if (world.runType == SOLO)
        {
            cout << "Using default ai-players=3" << endl;
            world.AIQty = 3;
        }

        if (vm.count("ipaddr"))
        {
            setAddr(vm["ipaddr"].as<string>().c_str());
        }
        else if (world.runType == CLIENT)
        {
            cout << "Using default ipaddr 127.0.0.1" << endl;
            setAddr("127.0.0.1");
        }
        if (vm.count("port"))
        {
            setPort(vm["port"].as<int>());
        }
        else if (world.runType == CLIENT)
        {
            cout << "Using default port 6888" << endl;
            setPort(6888);
        }
        if (vm.count("nox"))
        {
            world.nox = true;
        }
        if (vm.count("nosound"))
        {
            world.nosound = true;
        }
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
        return 2;
    }
    srand(time(NULL));

    if (!world.nox)
        graphics.initGraphics();
    if (!world.nosound)
        sound.initSound();

    if (world.runType == SOLO)
    {
        scheduler.welcomeScreen();
        world.makeAgents();
        testSetup();
        scheduler.soloLoopForever();
    }
    if (world.runType == CLIENT)
    {
        testSetup();
        if (client.connectToServer() < 0)
        {
            cerr << "Fatal error:  Cannot connect to server" << endl;
            return -1;
        }
        scheduler.welcomeScreen();
        scheduler.clientLoopForever();
    }
    if (world.runType == SERVER)
    {
        testSetup();
        if (server.createHost() < 0)
        {
            cerr << "Fatal error:  Server could not be established" << endl;
            return -1;
        }
        scheduler.serverLoopForever();
    }

    return 0;
}
