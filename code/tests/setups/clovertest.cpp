#include "Engine/setup.h"
#include "allclasses.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Utilities/vec3f.h"

void testSetup()
{
    World &world = World::getInstance();

    Vec3f pos2 = Vec3f(10, 20, 6);
    SphereInfo *sphere = new SphereInfo(1);
    Kinematic *k = new Kinematic(pos2, Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, sphere);
    GObject *gobj = new GObject(sphere);

    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    world.addObject(wobj);
}
