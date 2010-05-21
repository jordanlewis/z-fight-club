#include "allclasses.h"
#include "Engine/setup.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"

void testSetup()
{
    World &world = World::getInstance();

    ObjMeshInfo *mesh = new ObjMeshInfo("building/");
    Vec3f pos = Vec3f(50,0,-15);
    GObject *gobj = new GObject(mesh);
    WorldObject *wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("building2/");
    pos = Vec3f(79.0, 0.0, -17.0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);


    mesh = new ObjMeshInfo("building/");
    pos = Vec3f(112, 0.0, -5.0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("building2/");
    pos = Vec3f(50.0, 0.0, 25.0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);
    
    mesh = new ObjMeshInfo("building/");
    pos = Vec3f(135.0, 0.0, 40.0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

}
