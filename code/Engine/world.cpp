#include "world.h"
#include "Graphics/polygon.h"
#include "Graphics/gobject.h"
#include "Physics/pobject.h"
#include "Agents/player.h"
#include "Agents/ai.h"
#include "Engine/input.h"
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

    PGeom *geom;
    GObject *gobj;
    WorldObject *wobj;

    /* Create bottom plane */
    PlaneInfo info = PlaneInfo(0, 1, 0, -.1);
    geom = new PGeom(&info);
    wobj = new WorldObject(geom, NULL, NULL, NULL);
    addObject(wobj);

    /* Now create WorldObjects to represent the track */

    float depth = .1;
    float height = 2;
    float len;

    Vec2f_t xzwall;
    Vec3f_t wall;
    int i, j;
    float theta;
    Edge_t *e, *next;
    dQuaternion quat;
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
    wobj = new WorldObject(geom, gobj, NULL, NULL);
    addObject(wobj);

}

Agent *World::placeAgent(int place)
{
    if (!track)
        return NULL;
    int zp = place / 4;
    int xp = place % 2;
    int nsects = track->nSects;
    int backerSect = track->nSects - (zp + 1);
    int backSect = zp == 0 ? 0 : track->nSects - (zp);
    int frontSect = zp <= 1 ? 1 - zp : track->nSects - (zp - 1);
    Vec3f diff = Vec3f(track->verts[track->sects[frontSect].edges[0].start]) -
                 Vec3f(track->verts[track->sects[backSect].edges[0].start]);
    Vec3f pos = lerp(Vec3f(track->verts[track->sects[backSect].edges[0].start]),
                     Vec3f(track->verts[track->sects[backSect].edges[1].start]),
                     xp == 0 ? .33 : .66);
    pos += Vec3f(track->verts[track->sects[backerSect].edges[0].start]) -
           lerp(Vec3f(track->verts[track->sects[backSect].edges[0].start]),
                Vec3f(track->verts[track->sects[backerSect].edges[0].start]),
                place % 4 < 2 ? 0 : .5);
    pos[1] += 2;

    float orient = acos(Vec3f(0,0,1).dot(diff.unit()));

    Agent *agent = new Agent(pos, orient);
    addAgent(agent);
    Sound::getInstance().registerSource(wobjects.back(), new SObject("snore.wav", GetTime(), AL_TRUE));

    return agent;
}

void World::makeAI()
{
    if (!track)
        return;
    AIManager &ai = AIManager::getInstance();
    int nAgents = agents.size();
    Agent *agent = placeAgent(nAgents);
    ai.control(agent);
    ai.controllers.back()->lane((nAgents + 1) % 2);
}


void World::makePlayer()
{
    if (!track)
        return;

    Agent *agent = placeAgent(agents.size());
    camera = Camera(THIRDPERSON, agent);
    Sound::getInstance().registerListener(&camera);
    PlayerController *p = new PlayerController(agent);
    Input::getInstance().controlPlayer(p);
}

void World::makeAgents()
{
    if (PlayerQty == 1) makePlayer();
    for (int i = 0; i < AIQty; i++)
    {
        makeAI();
    }
}

void World::setRunType(const string str){
    Error error = Error::getInstance();
	
    if ( (str == "server") || (str == "Server") ){
	runType = SERVER;
    }
    else if ( (str =="client") || (str == "Client") ) {
	runType = CLIENT;
    }
    else if ( (str == "solo") || (str == "Solo") ) {
	runType = SOLO;
    }
    else {
	error.log(NETWORK, IMPORTANT,
		  "Unrecognized netmode. Defaulting to solo\n.");
	runType = SOLO;
    }
    return;
}

const TrackData_t *World::getTrack()
{
    return track;
}

World &World::getInstance()
{
    return _instance;
}

void World::setDir(const string dirname)
{
    assetsDir = dirname;
}
