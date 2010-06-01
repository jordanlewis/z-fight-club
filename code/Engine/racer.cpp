/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include <unistd.h>
#include  <sys/types.h>
#include "math.h"
#include "scheduler.h"
#include "Physics/physics.h"
#include "Graphics/graphics.h"
#include "Sound/sound.h"
#include "Sound/sobject.h"
#include "Network/network.h"
#include "Network/client.h"
#include "Network/server.h"
#include "Utilities/error.h"
#include "world.h"
#include "input.h"
#include "setup.h"

#include "Utilities/vec3f.h"

using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    int forkrv = fork();
    if (forkrv == 0) {
        execlp("mplayer", "mplayer", "../assets/CutScene/cutscene.ogv", NULL);
    } else {
        //sleep(20.8);
        po::variables_map vm;
        World &world = World::getInstance();
        Sound     &sound    = Sound::getInstance();
        Graphics  &graphics = Graphics::getInstance();
        Scheduler &scheduler = Scheduler::getInstance();
        Client &client = Client::getInstance();
        Server &server = Server::getInstance();

        srand(time(NULL));

        try {
            // Declare the supported options.
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "produce help message")
                ("track", po::value<string>(), "set track file")
                ("laps",  po::value<int>(),    "set number of laps in race")
                ("assets", po::value<string>(), "set assets directory")
                ("network", po::value<string>(), "set network mode")
                ("ipaddr", po::value<string>(), "set server ip address")
                ("port", po::value<int>(), "set server port")
                ("ai-players", po::value<int>(), "set number of AI players")
                ("nohuman", "disable human")
                ("nox", "disable graphics")
                ("nosound", "disable sound")
                ("nomusic", "disable music")
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
                ENGINE << TRIVIAL << "Using default track tests/tracks/oval.trk" << endl;
                world.loadTrack("tests/tracks/oval.trk");
            }

            if (vm.count("laps"))
            {
                world.nLaps = vm["laps"].as<int>();
            }
            else
            {
                ENGINE << TRIVIAL << "Using default lapcount 3" << endl;
                world.nLaps = 3;
            }

            if (vm.count("assets"))
            {
                world.setDir(vm["assets"].as<string>().c_str());
            }
            else
            {
                ENGINE << TRIVIAL << "Using default assets dir ../assets/" << endl;
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
            if (!vm.count("nohuman"))
            {
                world.PlayerQty = 1;
            }
            if (world.runType == SOLO || world.runType == SERVER)
            {
                if (vm.count("ai-players"))
                {
                    world.AIQty = vm["ai-players"].as<int>();
                    ((TextboxMenu *) world.setupMenu->items[1])->entered = 
                        boost::lexical_cast<string>(vm["ai-players"].as<int>());
                }
                else
                {
                    ENGINE << TRIVIAL << "Using default ai-players=3" << endl;
                    ((TextboxMenu *) world.setupMenu->items[1])->entered = "3";
                    world.AIQty = 3;
                }
            }
            else if (world.runType == CLIENT) 
            {
                ((TextboxMenu *) world.setupMenu->items[1])->entered = "0";
                world.AIQty = 0; //We will increment this as we get more AI.
            }

            if (vm.count("ipaddr"))
            {
                setAddr(vm["ipaddr"].as<string>().c_str());
                ((TextboxMenu *) ((SubMenu *) world.setupMenu->items[4])->items[1])->entered =
                    vm["ipaddr"].as<string>(); 
            }
            else if (world.runType == CLIENT)
            {
                ENGINE << TRIVIAL << "Using default ipaddr 127.0.0.1" << endl;
                setAddr("127.0.0.1");
                ((TextboxMenu *) ((SubMenu *) world.setupMenu->items[4])->items[1])->entered = "127.0.0.1";
            }
            if (vm.count("port"))
            {
                ((TextboxMenu * ) ((SubMenu *) world.setupMenu->items[4])->items[2])->entered =
                    boost::lexical_cast<string>(vm["port"].as<int>());
                setPort(vm["port"].as<int>());
            }
            else if ((world.runType == CLIENT) || (world.runType == SERVER))
            {
                ENGINE << TRIVIAL << "Using default port 6888" << endl;
                ((TextboxMenu * ) ((SubMenu *) world.setupMenu->items[4])->items[2])->entered = "6888"; 
                setPort(6888);
            }
            if (vm.count("nox"))
            {
                ((SelectorMenu *) ((SubMenu *) world.setupMenu->items[5])->items[1])->selected = 1;
                world.nox = true;
            }
            if (vm.count("nosound"))
            {
                ((SelectorMenu *) ((SubMenu *) world.setupMenu->items[5])->items[2])->selected = 1;
                world.nosound = true;
            }
            if (vm.count("nomusic"))
            {
                ((SelectorMenu *) ((SubMenu *) world.setupMenu->items[5])->items[3])->selected = 1;
                world.nomusic = true;
            }
        }
        catch(exception& e) {
            ENGINE << CRITICAL << "error: " << e.what() << "\n";
            return 1;
        }
        catch(...) {
            ENGINE << CRITICAL << "Exception of unknown type!\n";
            return 2;
        }


        if (!world.nox)
        {
            graphics.initGraphics();
            scheduler.setupLoopForever();
        }
        if (!world.nosound)
        {
            sound.initSound();
            if (!world.nomusic)
            {
                sound.addSoundAt("02 I Can See It In Your Face.wav", GetTime(), AL_TRUE, 0.2, Vec3f(0,0,0));
            }
        }

        if (world.runType == SOLO)
        {
            testSetup();
            world.makeAgents();
            world.makeSkybox();
            scheduler.soloLoopForever();
        }
        if (world.runType == CLIENT)
        {
            testSetup();
            if (client.connectToServer() < 0)
            {
                NETWORK << CRITICAL << "Fatal error:  Cannot connect to server" << endl;
                return -1;
            }
            world.makeSkybox();
            scheduler.clientLoopForever();
        }
        if (world.runType == SERVER)
        {
            testSetup();
            if (server.createHost() < 0)
            {
                NETWORK << CRITICAL << "Fatal error:  Server could not be established" << endl;
                return -1;
            }
            world.makeSkybox();
            scheduler.serverLoopForever();
        }

        return 0;
    }
}
