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

    Vec3f pos = Vec3f(0, 0, 0);
    Agent *agent = new Agent(pos, M_PI / 2);

    world.registerAgent(*agent);
    physics.initAgent(*agent);

    AIManager &ai = AIManager::getInstance();
    ai.control(*agent);

    ai.controllers[0]->path.knots.push(Vec3f(100, 100, 100));
    ai.controllers[0]->path.precision.push(0.5f);
}
