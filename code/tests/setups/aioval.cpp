#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Agents/ai.h"

void testSetup()
{
    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input = Input::getInstance();
    AIManager &ai = AIManager::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, 0, 0, 0, 0);
    new PGeom(&info, physics.getOdeSpace());

    Vec3f pos = Vec3f(62, 2, 10);
    Agent *aiagent = new Agent(pos, M_PI_2);

    Vec3f tgt = Vec3f(20, 2, 20);
    Agent *tgtagent = new Agent(tgt, M_PI_2);

    Vec3f pos3 = Vec3f(55, 2, 10);
    Agent *humanagent = new Agent(pos3, -1.337);

    world.registerAgent(*aiagent);
    physics.initAgent(*aiagent);
    world.registerAgent(*tgtagent);
    physics.initAgent(*tgtagent);
    world.registerAgent(*humanagent);
    physics.initAgent(*humanagent);

    ai.control(*aiagent);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(*humanagent);
    ///* Tell input to send input to that playerController */
    input.controlPlayer(*p);
}
