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

    //PlaneInfo info = PlaneInfo(0, 1, 0, 0, 0, 0, 0);
    //new PGeom(&info, physics.getOdeSpace());

    PlaneInfo info = PlaneInfo(0, 1, 0, 0);
    new PGeom(&info, physics.getOdeSpace());

    Vec3f pos = Vec3f(85, 5, 20);
    Agent *agent = new Agent(pos, M_PI);

    world.addAgent(*agent);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(*agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(*p);
}