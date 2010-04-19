/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"
#include "math.h"

#include "Utilities/vec3f.h"

using namespace std;

int main(int argc, char *argv[])
{
    World    &world    = World::getInstance();
    Graphics &graphics = Graphics::getInstance();
    Physics *physics = new Physics(&world);

    graphics.initGraphics();
    physics->initPhysics();

    Scheduler scheduler(&world, &graphics, physics);

    Vec3f pos = Vec3f(-8, .3, 0);
    Vec3f pos2 = Vec3f(30, 0, 0);
    Agent agent(pos, M_PI / 2);
    Agent agent2(pos2, -M_PI / 2);

    SteerInfo steer;
    steer.acceleration = .3;
    steer.rotation = 0;
    agent.setSteering(steer);
    steer.acceleration = 1;
    agent2.setSteering(steer);

    world.registerAgent(&agent);
    physics->initAgent(agent);
    world.registerAgent(&agent2);
    physics->initAgent(agent2);

    scheduler.loopForever();

    return 0;
}
