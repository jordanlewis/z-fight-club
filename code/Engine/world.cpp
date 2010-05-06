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

    TrackData_t *t = track;
    int i, pos;
    dTriMeshDataID floor = dGeomTriMeshDataCreate();
    dTriMeshDataID walls = dGeomTriMeshDataCreate();
    PGeom *geom;
    GObject *gobj;
    WorldObject *wobj;
    TriMeshInfo *tmeshinfo;
    Vec3f_t tmp1, tmp2;

    Vec3f_t *floorverts = new Vec3f_t[t->nVerts];
    Vec3f_t *wallverts  = new Vec3f_t[t->nVerts * 2];
    int *flooridxs = new int[t->nSects * 6];
    int *wallidxs  = new int[t->nSects * 24];
    Vec3f_t *wallnorms = new Vec3f_t[t->nSects * 8];

    memcpy(floorverts, t->verts, t->nVerts * sizeof(Vec3f_t));

    for (i = 0; i < t->nVerts; i++)
    {
        CopyV3f(t->verts[i], wallverts[i * 2]);
        CopyV3f(t->verts[i], wallverts[i * 2 + 1]);
        wallverts[i * 2 + 1][1] += 2;
    }

    for (i = 0; i < t->nSects; i++)
    {
        // two triangles per floor quad
        pos = 6 * i;
        flooridxs[pos]     = t->sects[i].edges[0].start;
        flooridxs[pos + 1] = t->sects[i].edges[1].start;
        flooridxs[pos + 2] = t->sects[i].edges[2].start;

        flooridxs[pos + 3] = t->sects[i].edges[2].start;
        flooridxs[pos + 4] = t->sects[i].edges[3].start;
        flooridxs[pos + 5] = t->sects[i].edges[0].start;


        /* 2 triangles per wall per sector
         * this assumes that the edge order of sectors always goes
         * entry, wall, exit, wall
         */
        pos = 24 * i;
        wallidxs[pos]      = t->sects[i].edges[0].start * 2;
        wallidxs[pos + 1]  = t->sects[i].edges[3].start * 2;
        wallidxs[pos + 2]  = t->sects[i].edges[3].start * 2 + 1;

        wallidxs[pos + 3]  = t->sects[i].edges[3].start * 2 + 1;
        wallidxs[pos + 4]  = t->sects[i].edges[0].start * 2 + 1;
        wallidxs[pos + 5]  = t->sects[i].edges[0].start * 2;

        wallidxs[pos + 6]  = t->sects[i].edges[1].start * 2;
        wallidxs[pos + 7]  = t->sects[i].edges[1].start * 2 + 1;
        wallidxs[pos + 8]  = t->sects[i].edges[2].start * 2 + 1;

        wallidxs[pos + 9]  = t->sects[i].edges[2].start * 2 + 1;
        wallidxs[pos + 10] = t->sects[i].edges[2].start * 2;
        wallidxs[pos + 11] = t->sects[i].edges[1].start * 2;

        /* now do the outside face: opposite winding */
        wallidxs[pos + 12] = t->sects[i].edges[0].start * 2;
        wallidxs[pos + 13] = t->sects[i].edges[0].start * 2 + 1;
        wallidxs[pos + 14] = t->sects[i].edges[3].start * 2 + 1;

        wallidxs[pos + 15] = t->sects[i].edges[3].start * 2 + 1;
        wallidxs[pos + 16] = t->sects[i].edges[3].start * 2;
        wallidxs[pos + 17] = t->sects[i].edges[0].start * 2;

        wallidxs[pos + 18] = t->sects[i].edges[1].start * 2;
        wallidxs[pos + 19] = t->sects[i].edges[2].start * 2;
        wallidxs[pos + 20] = t->sects[i].edges[2].start * 2 + 1;

        wallidxs[pos + 21] = t->sects[i].edges[2].start * 2 + 1;
        wallidxs[pos + 22] = t->sects[i].edges[1].start * 2 + 1;
        wallidxs[pos + 23] = t->sects[i].edges[1].start * 2;


        /* Calculate normals */
        for (int j = 0; j < 4; j++)
        {
            SubV3f(t->verts[wallidxs[pos + 1]], t->verts[wallidxs[pos]], tmp1);
            SubV3f(t->verts[wallidxs[pos + 2]], t->verts[wallidxs[pos]], tmp2);
            CrossV3f(tmp1, tmp2, wallnorms[4 * i + j]);
            ZeroV3f(tmp1);
            NormalizeV3f(wallnorms[4 * i + j]);
            SubV3f(tmp1, wallnorms[4 * i + j], wallnorms[4 * i + j + 4]);
            pos += 3;
        }
    }
    dGeomTriMeshDataBuildSingle(floor,
                                t->verts, sizeof(Vec3f_t), t->nVerts,
                                flooridxs, t->nSects * 6, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(floor, t->nVerts, floorverts,
                                       t->nSects * 6, flooridxs,
                                       NULL);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    wobj = new WorldObject(geom, gobj, NULL, NULL);
    addObject(wobj);

    dGeomTriMeshDataBuildSingle(walls,
                                wallverts, sizeof(Vec3f_t), t->nVerts * 2,
                                wallidxs, t->nSects * 24, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(walls, t->nVerts * 2, wallverts,
                               t->nSects * 24, wallidxs,
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
