#include "Engine/setup.h"
#include "allclasses.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"
#include "Utilities/vec3f.h"

void testSetup()
{
    World &world = World::getInstance();

    Vec3f pos = Vec3f(0.0, 0.0, 0.0);

    SkyBoxInfo *sbox = new SkyBoxInfo("skybox/red/");
    Kinematic *k = new Kinematic(pos, Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, sbox);
    GObject *gobj = new GObject(sbox);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    world.addObject(wobj);
}
