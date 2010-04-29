#include "world.h"
#include "Graphics/polygon.h"
#include "Graphics/gobject.h"
#include "Physics/pobject.h"
extern "C" {
    #include "Parser/track-parser.h"
}

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World World::_instance;

WorldObject::WorldObject(PGeom *pobject, GObject *gobject, Agent *agent)
                        : pobject(pobject), gobject(gobject), agent(agent)
{}

Vec3f WorldObject::getPos()
{
    return pobject->getPos();
}

void WorldObject::getQuat(Quatf_t quat)
{
    pobject->getQuat(quat);
}

void WorldObject::draw()
{
    if (gobject == NULL)
        return;
    Quatf_t quat;
    getQuat(quat);
    gobject->draw(getPos(), quat);
}

World::World()
            : environment(std::vector<Polygon>()), agents(std::vector<Agent*>())
{}

World::~World()
{
    FreeTrackData(track);
}

void World::addObject(WorldObject obj)
{
    wobjects.push_back(WorldObject(obj.pobject, obj.gobject, obj.agent));
}

void World::registerAgent(Agent &agent)
{
    agents.push_back(&agent);
}

void World::loadTrack(const char *file)
{
    track = LoadTrackData(file);
    if (!track)
        return; /* error */

    /* Now create WorldObjects to represent the track */

    float depth = .1;
    float height = 2;
    float len;

    Vec2f_t xzwall;
    Vec3f_t wall;
    int i, j;
    float theta;
    Edge_t *e, *next;
    PGeom *geom;
    GObject *gobj;
    dQuaternion quat;
    Vec3f position;

    for (i = 0; i < track->nSects; i++)
    {
        /* for each edge in every sector, if its a wall edge, create a box
         * that represents the wall
         */
        for (j = 0; j < track->sects[i].nEdges; j++)
        {
            e = track->sects[i].edges + j;
            if (j == track->sects[i].nEdges - 1)
                next = e - 3;
            else
                next = e + 1;
            if (e->kind == WALL_EDGE)
            {
                SubV3f(track->verts[e->start],track->verts[next->start], wall);
                xzwall[0] = wall[0];
                xzwall[1] = wall[2];
                len = LengthV2f(xzwall);

                /* this bit makes a pgeom and sets its position and rotation */
                BoxInfo *box = new BoxInfo(len, height + wall[1], depth);
                theta = atan2(wall[2], wall[0]);
                geom = new PGeom(box);
                geom->bounce = 1;
                dQFromAxisAndAngle(quat, 0, 1, 0, -theta);
                geom->setQuat(quat);
                LerpV3f(track->verts[e->start], .5, track->verts[next->start],
                        wall);
                position = Vec3f(wall[0], wall[1], wall[2]);
                geom->setPos(position);

                /* now we make a corresponding gobject */
                gobj = new GObject(box);

                addObject(WorldObject(geom, gobj, NULL));
            }
        }
    }

}

const TrackData_t *World::getTrack()
{
    return track;
}

World &World::getInstance() 
{
    return _instance;
}
