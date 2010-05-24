#include "Engine/setup.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Utilities/vec3f.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"
#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();
    error.setVerbosity(TRIVIAL);
    World &world = World::getInstance();

    Vec3f position = Vec3f(40.0, 2.0, 8.0);
    Vec3f area = Vec3f(1.0, 1.0, 1.0);
    Vec3f velocity = Vec3f(-1.0, 0.0, 0.0);
    Vec3f velocity_pm = Vec3f(1.0, 1.0, 1.0);
    float ttl = 5.0;
    float ttl_pm = 0.0;
    float birthRate = 100.0;

    ParticleSystemInfo *particleSystem = new ParticleSystemInfo("particles/default.png", area, velocity, velocity_pm, ttl, ttl_pm, birthRate);
    Kinematic *k = new Kinematic(position, Vec3f(0,0,0), M_PI/2);
    GObject *gobj = new GObject(particleSystem);
    WorldObject *wobj = new WorldObject(NULL, gobj, NULL, NULL);

    wobj->setPos(position);
    world.addObject(wobj);
}
