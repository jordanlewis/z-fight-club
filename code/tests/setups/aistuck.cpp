#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Agents/ai.h"

void testSetup()
{
    World &world  = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input  = Input::getInstance();
    AIManager &ai = AIManager::getInstance();
    Sound &sound  = Sound::getInstance();


    Vec3f pos = Vec3f(29,1,9.4);
    float orient = 0;
    Agent *agent = new Agent(pos, orient);
    world.addAgent(agent);
    ai.control(agent);
    ai.controllers.back()->lane(1);
}
