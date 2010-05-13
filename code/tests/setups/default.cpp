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
    Sound &sound = Sound::getInstance();

    Vec3f pos = Vec3f(85, 5, 20);
    Agent *agent = new Agent(pos, M_PI);

    world.addAgent(agent);
    world.camera = Camera(THIRDPERSON,agent);
    sound.registerListener(&world.camera);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(p);
}
