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

    Vec3f pos = Vec3f(44, 2, 6);
    Agent *agent = new Agent(pos, M_PI / 2);

    world.addAgent(*agent);

    AIManager &ai = AIManager::getInstance();
    ai.control(*agent);

    ai.controllers[0]->lane(0);
}
