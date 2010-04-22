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
#include "Utilities/defs.h"

using namespace std;

int main(int argc, char *argv[])
{
    World &world = World::getInstance();
    if (argc > 1)
        world.loadTrack(argv[1]);

    Graphics  &graphics = Graphics::getInstance();
    Sound     &sound    = Sound::getInstance();
    Physics   &physics  = Physics::getInstance();
    Scheduler &scheduler = Scheduler::getInstance();

    graphics.initGraphics();
    sound.initSound();

    if (world.getTrack() != NULL)
        physics.makeTrackGeoms();

    testSetup();

    // sound.schedule_sound("test.wav", GetTime()+10, Vec3f(0,0,0));

    scheduler.loopForever();

    return 0;
}
