/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "math.h"
#include "scheduler.h"
#include "Agents/input.h"
#include "Physics/physics.h"
#include "Graphics/graphics.h"
#include "Sound/sound.h"

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
    Input     &input = Input::getInstance();


    graphics.initGraphics();
    sound.initSound();
    physics.initPhysics();


    Vec3f pos = Vec3f(-8, .3, .3);
    Vec3f pos2 = Vec3f(30, 0, 0);
    Agent agent(pos, M_PI / 2);
    Agent agent2(pos2, -M_PI / 2);

    SteerInfo steer;
    steer.acceleration = .3;
    steer.rotation = 0;
    agent.setSteering(steer);
    steer.acceleration = 1;
    agent2.setSteering(steer);

    world.registerAgent(agent);
    physics.initAgent(agent);
    world.registerAgent(agent2);
    physics.initAgent(agent2);

    input.controlAgent(&agent);
    // sound.schedule_sound("test.wav", GetTime()+10, Vec3f(0,0,0));

    scheduler.loopForever();

    return 0;
}
