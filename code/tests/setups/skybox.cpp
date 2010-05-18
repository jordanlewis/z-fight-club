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

    Vec3f pos = Vec3f(40.0, 10.0, 8.0);

    SkyBoxInfo *sbox = new SkyBoxInfo("skybox/red/");
    Kinematic *k = new Kinematic(pos, Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, sbox);
    GObject *gobj = new GObject(sbox);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    world.addObject(wobj);
}
