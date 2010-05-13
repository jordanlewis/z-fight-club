#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Graphics/camera.h"

void testSetup()
{
    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input = Input::getInstance();

    Vec3f pos = Vec3f(2,10,2);
    Agent *agent = new Agent(pos, M_PI / 2);

    world.addAgent(agent);
 
    Vec3f pos2 = Vec3f(10, 20, 6);
    SphereInfo *sphere = new SphereInfo(1);
    Kinematic *k = new Kinematic(pos2, Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, sphere);
    GObject *gobj = new GObject(sphere);

    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    world.addObject(wobj);

    world.camera = Camera(THIRDPERSON, agent);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(p);
}
