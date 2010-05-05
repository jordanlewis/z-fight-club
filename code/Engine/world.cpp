#include "world.h"
#include "Graphics/polygon.h"
#include "Graphics/gobject.h"
#include "Physics/pobject.h"
#include "Sound/sobject.h"
#include "Utilities/error.h"
#include <ode/ode.h>
extern "C" {
    #include "Parser/track-parser.h"
}

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World World::_instance;

WorldObject::WorldObject(PGeom *pobject, GObject *gobject, SObject *sobject, Agent *agent)
    : pobject(pobject), gobject(gobject), sobject(sobject), agent(agent)
{
    pos = Vec3f(-1,-1,-1);
    if (pobject != NULL)
	{
	    pobject->worldObject = this;
	}
}

Vec3f WorldObject::getPos()
{
    if (pobject) return pobject->getPos();
    return pos;
}

void WorldObject::setPos(Vec3f position)
{
    if (pobject) pobject->setPos(position);
    pos = position;
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
    if (agent == NULL)
        gobject->draw(getPos(), quat);
    else
        gobject->draw(getPos(), quat, agent);
}

World::World()
            : environment(std::vector<Polygon>()), agents(std::vector<Agent*>())
{}

World::~World()
{
    FreeTrackData(track);
}

void World::addObject(WorldObject *obj)
{
    wobjects.push_back(obj);
}

void World::addAgent(Agent *agent)
{
    BoxInfo *box = new BoxInfo(agent->width, agent->height, agent->depth);
    PAgent *pobj = new PAgent(&(agent->getKinematic()), &(agent->getSteering()),
                              agent->mass, box);
    pobj->bounce = 1;
    GObject *gobj = new GObject(box);
    SObject *sobj = NULL;

    WorldObject *wobject = new WorldObject(pobj, gobj, sobj, agent);

    addObject(wobject);

    Physics::getInstance().getAgentMap()[agent->id] = pobj;

    agents.push_back(agent);
}

void World::loadTrack(const char *file)
{
    track = LoadTrackData(file);
    Error error = Error::getInstance();
    if (!track) {
        error.log(PARSER, CRITICAL, "Track load failed\n");
    }

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
    dQuaternion quat, quattmp;
    Vec3f position, diff;

    int *indices = new int[track->nSects * 6];
    dTriMeshDataID tmid = dGeomTriMeshDataCreate();
    for (i = 0; i < track->nSects; i++)
    {
        /* for each edge in every sector, if its a wall edge, create a box
         * that represents the wall
         */
        if (track->sects[i].nEdges != 4)
        {
            error.log(ENGINE, CRITICAL, "Non-rect sectors unsupported\n");
            return;
        }

        indices[6 * i]     = track->sects[i].edges[0].start;
        indices[6 * i + 1] = track->sects[i].edges[1].start;
        indices[6 * i + 2] = track->sects[i].edges[2].start;
        indices[6 * i + 3] = track->sects[i].edges[2].start;
        indices[6 * i + 4] = track->sects[i].edges[3].start;
        indices[6 * i + 5] = track->sects[i].edges[0].start;

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
                BoxInfo *box = new BoxInfo(len, abs(height + wall[1]), depth);
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

                WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);

                addObject(wobj);
            }
        }
    }
    dGeomTriMeshDataBuildSingle(tmid,
                                track->verts, sizeof(Vec3f_t), track->nVerts,
                                indices, track->nSects * 6, sizeof(int) * 3);
    TriMeshInfo *tmeshinfo = new TriMeshInfo(tmid, track->nVerts, track->verts,
                                             track->nSects * 6, indices, NULL);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    addObject(wobj);

}

const TrackData_t *World::getTrack()
{
    return track;
}

World &World::getInstance()
{
    return _instance;
}
