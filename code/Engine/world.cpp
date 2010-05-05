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

    int i, pos;
    dTriMeshDataID floor = dGeomTriMeshDataCreate();
    dTriMeshDataID walls = dGeomTriMeshDataCreate();
    PGeom *geom;
    GObject *gobj;
    WorldObject *wobj;
    TriMeshInfo *tmeshinfo;
    Vec3f_t tmp1, tmp2;

    Vec3f_t *floorverts = new Vec3f_t[track->nVerts];
    Vec3f_t *wallverts  = new Vec3f_t[track->nVerts * 2];
    int *flooridxs = new int[track->nSects * 6];
    int *wallidxs  = new int[track->nSects * 12];
    Vec3f_t *wallnorms = new Vec3f_t[track->nSects * 4];

    memcpy(floorverts, track->verts, track->nVerts * sizeof(Vec3f_t));

    for (i = 0; i < track->nVerts; i++)
    {
        CopyV3f(track->verts[i], wallverts[i * 2]);
        CopyV3f(track->verts[i], wallverts[i * 2 + 1]);
        wallverts[i * 2 + 1][1] += 2;
    }

    for (i = 0; i < track->nSects; i++)
    {
        // two triangles per floor quad
        pos = 6 * i;
        flooridxs[pos]     = track->sects[i].edges[0].start;
        flooridxs[pos + 1] = track->sects[i].edges[1].start;
        flooridxs[pos + 2] = track->sects[i].edges[2].start;

        flooridxs[pos + 3] = track->sects[i].edges[2].start;
        flooridxs[pos + 4] = track->sects[i].edges[3].start;
        flooridxs[pos + 5] = track->sects[i].edges[0].start;


        /* 2 triangles per wall per sector
         * this assumes that the edge order of sectors always goes
         * entry, wall, exit, wall
         */
        pos = 12 * i;
        wallidxs[pos]      = track->sects[i].edges[0].start * 2;
        wallidxs[pos + 1]  = track->sects[i].edges[3].start * 2;
        wallidxs[pos + 2]  = track->sects[i].edges[3].start * 2 + 1;

        SubV3f(track->verts[wallidxs[pos + 1]], track->verts[wallidxs[pos]], tmp1);
        SubV3f(track->verts[wallidxs[pos + 2]], track->verts[wallidxs[pos]], tmp2);
        CrossV3f(tmp1, tmp2, wallnorms[4 * i]);

        wallidxs[pos + 3]  = track->sects[i].edges[3].start * 2 + 1;
        wallidxs[pos + 4]  = track->sects[i].edges[0].start * 2 + 1;
        wallidxs[pos + 5]  = track->sects[i].edges[0].start * 2;
        SubV3f(track->verts[wallidxs[pos + 4]], track->verts[wallidxs[pos + 3]], tmp1);
        SubV3f(track->verts[wallidxs[pos + 5]], track->verts[wallidxs[pos + 3]], tmp2);
        CrossV3f(tmp1, tmp2, wallnorms[4 * i + 1]);

        wallidxs[pos + 6]  = track->sects[i].edges[1].start * 2;
        wallidxs[pos + 7]  = track->sects[i].edges[1].start * 2 + 1;
        wallidxs[pos + 8]  = track->sects[i].edges[2].start * 2 + 1;
        SubV3f(track->verts[wallidxs[pos + 6]], track->verts[wallidxs[pos + 6]], tmp1);
        SubV3f(track->verts[wallidxs[pos + 7]], track->verts[wallidxs[pos + 6]], tmp2);
        CrossV3f(tmp1, tmp2, wallnorms[4 * i + 2]);

        wallidxs[pos + 9]  = track->sects[i].edges[2].start * 2 + 1;
        wallidxs[pos + 10] = track->sects[i].edges[2].start * 2;
        wallidxs[pos + 11] = track->sects[i].edges[1].start * 2;
        SubV3f(track->verts[wallidxs[pos + 10]], track->verts[wallidxs[pos + 9]], tmp1);
        SubV3f(track->verts[wallidxs[pos + 11]], track->verts[wallidxs[pos + 9]], tmp2);
        CrossV3f(tmp1, tmp2, wallnorms[4 * i + 3]);
    }
    dGeomTriMeshDataBuildSingle(floor,
                                track->verts, sizeof(Vec3f_t), track->nVerts,
                                flooridxs, track->nSects * 6, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(floor, track->nVerts, floorverts,
                                       track->nSects * 6, flooridxs,
                                       NULL);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    wobj = new WorldObject(geom, gobj, NULL, NULL);
    addObject(wobj);

    dGeomTriMeshDataBuildSingle(walls,
                                wallverts, sizeof(Vec3f_t), track->nVerts * 2,
                                wallidxs, track->nSects * 12, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(walls, track->nVerts * 2, wallverts,
                               track->nSects * 12, wallidxs,
                               wallnorms);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    wobj = new WorldObject(geom, gobj, NULL, NULL);
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
