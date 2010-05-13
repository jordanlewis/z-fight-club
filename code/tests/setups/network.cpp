#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();
    error.on(NETWORK);

    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input = Input::getInstance();

    Vec3f pos2 = Vec3f(25, 2, 4.5);
    Agent *humanagent = new Agent(pos2, M_PI_2);
    world.addAgent(humanagent);
    world.camera = Camera(THIRDPERSON, humanagent);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(humanagent);
    ///* Tell input to send input to that playerController */
    input.controlPlayer(p);
}
