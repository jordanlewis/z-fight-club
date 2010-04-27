#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"

void testSetup()
{
    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input = Input::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, 0, 0, 0, 0);
    new PGeom(&info, physics.getOdeSpace());

    Vec3f pos = Vec3f(-8, 20, .3);
    Vec3f pos2 = Vec3f(30, 25, 0);
    Agent *agent = new Agent(pos, M_PI / 2);
    Agent *agent2 = new Agent(pos2, -M_PI / 2);

    SteerInfo steer;
    steer.acceleration = .3;
    steer.rotation = 0;
    agent->setSteering(steer);
    steer.acceleration = 1;
    agent2->setSteering(steer);

    world.registerAgent(*agent);
    physics.initAgent(*agent);
    world.registerAgent(*agent2);
    physics.initAgent(*agent2);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(*agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(*p);
}
