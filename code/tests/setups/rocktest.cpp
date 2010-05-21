#include "Engine/setup.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Utilities/vec3f.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"

void testSetup()
{
    World &world = World::getInstance();

    Vec3f pos = Vec3f(40.0, 10.0, 8.0);

    ObjMeshInfo *mesh = new ObjMeshInfo("rock/");
    Kinematic *k = new Kinematic(pos, Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, mesh);
    GObject *gobj = new GObject(mesh);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    world.addObject(wobj);
}
