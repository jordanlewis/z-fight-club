#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Engine/scheduler.h"
#include "Utilities/vec3f.h"
#include "Agents/ai.h"

void testSetup()
{
    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input = Input::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, 0);
    new PGeom(&info, physics.getOdeSpace());

    Vec3f pos = Vec3f(25, 2, 7.5);
    Agent *agent = new Agent(pos, M_PI / 2);

    world.addAgent(agent);
    world.camera = Camera(BIRDSEYE, agent);

    AIManager &ai = AIManager::getInstance();
    ai.control(agent);

    ai.controllers[0]->lane(1);
    /* Vec3f obstacle1 = Vec3f(35, 0, 8.0);

    ai.controllers[0]->avoid(obstacle1); 

    ai.controllers[0]->obstacles[0].ttl = 100.0f;
    ai.controllers[0]->obstacles[0].str = 100.0f; */
}
