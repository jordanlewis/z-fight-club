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
    Physics   &physics  = Physics::getInstance();
    Scheduler &scheduler = Scheduler::getInstance();

    try {
        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("track", po::value<string>(), "set track file")
            ("sounds", po::value<string>(), "set sounds directory")
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
    
        if (vm.count("sounds"))
        {
            sound.setDir(vm["sounds"].as<string>().c_str());
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

    graphics.initGraphics();
    sound.initSound();

    if (world.getTrack() != NULL)
        physics.makeTrackGeoms();

    testSetup();

    scheduler.loopForever();

    return 0;
}
