#include "allclasses.h"
#include "Engine/setup.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"

void testSetup()
{
    World &world = World::getInstance();

    ObjMeshInfo *mesh = new ObjMeshInfo("building/");
    Kinematic *k = new Kinematic(Vec3f(50.0, 0.0, -15.0), Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, mesh);
    GObject *gobj = new GObject(mesh);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);

    world.addObject(wobj);
    
    mesh = new ObjMeshInfo("building/");
    k = new Kinematic(Vec3f(79.0, 0.0, -17.0), Vec3f(0,0,0), M_PI/2);
    geom = new PMoveable(k, 10, mesh);
    gobj = new GObject(mesh);
    wobj = new WorldObject(geom, gobj, NULL, NULL);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("building/");
    k = new Kinematic(Vec3f(112, 0.0, -5.0), Vec3f(0,0,0), M_PI/2);
    geom = new PMoveable(k, 10, mesh);
    gobj = new GObject(mesh);
    wobj = new WorldObject(geom, gobj, NULL, NULL);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("building/");
    k = new Kinematic(Vec3f(50.0, 0.0, 25.0), Vec3f(0,0,0), M_PI/2);
    geom = new PMoveable(k, 10, mesh);
    gobj = new GObject(mesh);
    wobj = new WorldObject(geom, gobj, NULL, NULL);

    world.addObject(wobj);
    
    mesh = new ObjMeshInfo("building/");
    k = new Kinematic(Vec3f(135.0, 0.0, 40.0), Vec3f(0,0,0), M_PI/2);
    geom = new PMoveable(k, 10, mesh);
    gobj = new GObject(mesh);
    wobj = new WorldObject(geom, gobj, NULL, NULL);

    world.addObject(wobj);

}
