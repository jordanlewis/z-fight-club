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

    /* Vec3f obstacle1 = Vec3f(35, 0, 8.0);

    ai.controllers[0]->avoid(obstacle1); 

    ai.controllers[0]->obstacles[0].ttl = 100.0f;
    ai.controllers[0]->obstacles[0].str = 100.0f; */
}
