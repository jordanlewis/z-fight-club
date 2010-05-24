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

    Vec3f pos = Vec3f(0.0, 0.0, 0.0);

    //ParticleInfo *particle = new ParticleInfo("particles/default.png");
    Kinematic *k = new Kinematic(pos, Vec3f(0,0,0), M_PI/2);
    GObject *gobj = new GObject(particle);
    WorldObject *wobj = new WorldObject(NULL, gobj, NULL, NULL);
    world.addObject(wobj);
}
