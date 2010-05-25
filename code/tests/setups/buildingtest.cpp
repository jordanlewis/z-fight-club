#include "allclasses.h"
#include "Engine/setup.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Graphics/gobject.h"
#include "Agents/kinematic.h"

void testSetup()
{
    World &world = World::getInstance();

    ObjMeshInfo *mesh = new ObjMeshInfo("Track/buildings1/");
    Vec3f pos = Vec3f(0,0,0);
    GObject *gobj = new GObject(mesh);
    WorldObject *wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("Track/buildings2/");
    pos = Vec3f(0, 0, 0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);


    mesh = new ObjMeshInfo("Track/flags/");
    pos = Vec3f(0, 0, 0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

    mesh = new ObjMeshInfo("Track/supports/");
    pos = Vec3f(0, 0, 0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);
    
    mesh = new ObjMeshInfo("Track/track/");
    pos = Vec3f(0, 0, 0);
    gobj = new GObject(mesh);
    wobj = new WorldObject(NULL, gobj, NULL, NULL);
    wobj->setPos(pos);

    world.addObject(wobj);

}
