/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"

#include "../Utilities/vec3f.h"

using namespace std;

int main(int argc, char *argv[])
{
    World *world = new World;
    Graphics *graphics = new Graphics;
    Physics *physics = new Physics(world);

    graphics->InitGraphics();
    physics->initPhysics();

    Scheduler scheduler(world, graphics, physics);

    Vec3f pos = Vec3f(100, 0, 100);
    Agent agent(pos);

    SteerInfo steer;
    steer.acceleration = 0.001;
    steer.rotation = 0;
    agent.setSteering(steer);

    world->registerAgent(agent);
    physics->initAgent(agent);



    scheduler.loopForever();

    return 0;
}
