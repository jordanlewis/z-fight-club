#include "world.h"
#include "Graphics/polygon.h"
#include "Graphics/gobject.h"
#include "Physics/pobject.h"
#include "Graphics/hud.h"
#include "Agents/player.h"
#include "Agents/agent.h"
#include "Agents/ai.h"
#include "Engine/input.h"
#include "Sound/sound.h"
#include "Utilities/error.h"
#include "Engine/geominfo.h"
#include "Sound/sobject.h"
#include <ode/ode.h>
extern "C" {
    #include "Parser/track-parser.h"
}

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World World::_instance;

WorldObject::WorldObject(PGeom *pobject, GObject *gobject, SObject *sobject, Agent *agent)
    : pobject(pobject), gobject(gobject), sobject(sobject), agent(agent), player(NULL)
{
    pos = Vec3f(-1,-1,-1);
    Quatf_t newquat = {0,0,0,1};
    CopyV3f(newquat, quat);
    if (pobject != NULL)
    {
        pobject->worldObject = this;
    }
    if (agent != NULL)
    {
        agent->worldObject = this;
    }

    parent = NULL;
}

Vec3f WorldObject::getPos()
{
    if (pobject && pobject->isPlaceable()) return pobject->getPos();
    return pos;
}

void WorldObject::setPos(Vec3f position)
{
    if (pobject) pobject->setPos(position);
    pos = position;
}

void WorldObject::getQuat(Quatf_t quat)
{
    if (pobject && pobject->isPlaceable())
    {
        pobject->getQuat(quat);
    }
    else
    {
        CopyQuatf(this->quat, quat);
    }
}

void WorldObject::setQuat(Quatf_t quat)
{
    if (pobject) pobject->setQuat(quat);
    CopyQuatf(quat, this->quat);
}

void WorldObject::draw()
{
    if (gobject == NULL)
        return;
    Quatf_t quat;
    getQuat(quat);
    if (agent == NULL) {
        Vec3f pos = getPos();
        if (parent != NULL)
            pos += parent->getPos();
        gobject->draw(pos, quat);
    } else
        gobject->draw(getPos(), quat, agent);
}

ParticleStreamObject::ParticleStreamObject(PGeom *pobject, GParticleObject *gobject, SObject *sobject,
  Agent *agent) : WorldObject(pobject,gobject,sobject,agent), gobject(gobject)
{
    // As with GParticleObject, we now have two pointers to the same piece of data, but the advantage
    // of this is that we get extra type info with it (see the note under GParticleObject)
}

World::World() :
    error(&Error::getInstance()), nox(false), nosound(false)
{
}

Light::Light()
{
    //pos = { 10.0, 10.0, -10.0, 1.0 };
    //lightColor = { 1.0, 1.0, 1.0, 1.0 };
    //ambientColor = { 0.2, 0.2, 0.2, 1.0 };
    //specColor = { .2, .2, .2, 1.0 };
}

Light::Light(GLfloat *pos, GLfloat *lightColor, GLfloat *ambientColor, GLfloat *specColor)
{
    int i;

    for (i = 0; i < 4; i++) {
        this->pos[i] = pos[i];
        this->lightColor[i] = lightColor[i];
        this->ambientColor[i] = ambientColor[i];
        this->specColor[i] = specColor[i];
    }
}

Light::~Light()
{}

void Light::setup(GLenum lightEnum)
{
    glLightfv(lightEnum, GL_POSITION, pos);
    glLightfv(lightEnum, GL_AMBIENT, lightColor);
    glLightfv(lightEnum, GL_SPECULAR, ambientColor);
    glLightfv(lightEnum, GL_DIFFUSE, specColor);
    glEnable(lightEnum);
}

World::~World()
{
    FreeTrackData(track);
}

void World::addObject(WorldObject *obj)
{
    wobjects.push_back(obj);
}

void World::addObject(ParticleStreamObject *obj)
{
    particleSystems.push_back(obj);
}

void World::addLight(Light *light)
{
    lights.push_back(light);
}

void World::addWidget(Widget *widget)
{
    widgets.push_back(widget);
}

int World::numAgents()
{
    int num = 0;
    for (vector<WorldObject *>::iterator iter = wobjects.begin(); iter != wobjects.end();
         iter++)
    {
        if ((*iter)->agent)
            num++;
    }
    return num;
}

void World::addAgent(Agent *agent)
{
    BoxInfo *box = new BoxInfo(agent->width, agent->height, agent->depth);
    ObjMeshInfo *ship = new ObjMeshInfo("ship/"); 
    PAgent *pobj = new PAgent(&(agent->getKinematic()), &(agent->getSteering()),
                              agent->mass, box);
    pobj->bounce = 1;
    GObject *gobj = new GObject(ship);
    SObject *sobj = new SObject("18303_start.wav", GetTime(), AL_FALSE);
    sobj->registerNext(new SObject("18303_run.wav", 0, AL_TRUE));

    WorldObject *wobject = new WorldObject(pobj, gobj, sobj, agent);

    addObject(wobject);


    /* create a particle generator for the agent */
    Vec3f position = Vec3f(0.0, .5, 0.0);
    Vec3f area = Vec3f(.01, .01, .01);
    Vec3f velocity = Vec3f(-1.0, 0.0, 0.0);
    Vec3f velocity_pm = Vec3f(0, .3, .3);
    float ttl = 2.0;
    float ttl_pm = 1.0;
    float birthRate = 200.0;

    ParticleSystemInfo *particleSystem = new ParticleSystemInfo("particles/beam.png", area, velocity, velocity_pm, ttl, ttl_pm, birthRate);
    GParticleObject *particle_gobj = new GParticleObject(particleSystem);
    ParticleStreamObject *particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL);
    particle_wobj->parent = wobject;
    particle_wobj->setPos(position);


    addObject(particle_wobj);
}

void World::loadTrack(const char *file)
{
    track = LoadTrackData(file);
    if (!track) {
        error->log(PARSER, CRITICAL, "Track load failed\n");
    }

    PGeom *geom;
    GObject *gobj;
    WorldObject *wobj;

    /* Create bottom plane */
    PlaneInfo info = PlaneInfo(0, 1, 0, -.1);
    geom = new PGeom(&info);
    wobj = new WorldObject(geom, NULL, NULL, NULL);
    addObject(wobj);


    TrackData_t *t = track;
    int i, pos;
    dTriMeshDataID floor = dGeomTriMeshDataCreate();
    dTriMeshDataID walls = dGeomTriMeshDataCreate();
    TriMeshInfo *tmeshinfo;
    Vec3f_t tmp1, tmp2;

    Vec3f_t *floorverts = new Vec3f_t[t->nVerts];
    Vec3f_t *wallverts  = new Vec3f_t[t->nVerts * 2];
    int *flooridxs = new int[t->nSects * 6];
    int *wallidxs  = new int[t->nSects * 12];
    Vec3f_t *wallnorms = new Vec3f_t[t->nSects * 4];

    memcpy(floorverts, t->verts, t->nVerts * sizeof(Vec3f_t));

    for (i = 0; i < t->nVerts; i++)
    {
        /* The odd numbered indices are above the even numbered ones - these
         * are the tops of the walls. */
        CopyV3f(t->verts[i], wallverts[i * 2]);
        CopyV3f(t->verts[i], wallverts[i * 2 + 1]);
        wallverts[i * 2 + 1][1] += 1;
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
        pos = 12 * i;
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


    }

    for (int i = 0; i < t->nSects * 4 / 3; i++)
    {
        SubV3f(wallverts[wallidxs[i*3 + 1]], wallverts[wallidxs[i * 3]], tmp1);
        SubV3f(wallverts[wallidxs[i*3 + 2]], wallverts[wallidxs[i * 3]], tmp2);
        CrossV3f(tmp1, tmp2, tmp1);
        memcpy(wallnorms + i * 3, tmp1, sizeof(float) * 3);
        memcpy(wallnorms + i * 3 + 1, tmp1, sizeof(float) * 3);
        memcpy(wallnorms + i * 3 + 2, tmp1, sizeof(float) * 3);
    }

    dGeomTriMeshDataBuildSingle(floor,
                                t->verts, sizeof(Vec3f_t), t->nVerts,
                                flooridxs, t->nSects * 6, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(floor, t->nVerts, floorverts,
                                       t->nSects * 6, flooridxs,
                                       NULL);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    //with trimesh geometry
    /* wobj = new WorldObject(geom, gobj, NULL, NULL); */

    //without trimesh geometry
    wobj = new WorldObject(geom, NULL, NULL, NULL);
    addObject(wobj);
    floorObj = wobj;

    dGeomTriMeshDataBuildSingle(walls,
                                wallverts, sizeof(Vec3f_t), t->nVerts * 2,
                                wallidxs, t->nSects * 12, sizeof(int) * 3);
    tmeshinfo = new TriMeshInfo(walls, t->nVerts * 2, wallverts,
                               t->nSects * 12, wallidxs,
                               wallnorms);
    geom = new PGeom(tmeshinfo);
    gobj = new GObject(tmeshinfo);
    wobj = new WorldObject(geom, NULL, NULL, NULL);
    addObject(wobj);


    /* Finally, create the lap path for the track. */
    path.clear();
    Vec3f midpoint;
    for (int i = 0; i < t->nSects; i++)
    {
        midpoint = lerp(Vec3f(t->verts[t->sects[i].edges[0].start]),
                        Vec3f(t->verts[t->sects[i].edges[1].start]), .5);
        path.knots.push_back(midpoint);
        path.precision.push_back(2);
    }
    path.computeDistances();

    MiniMap *mm = new MiniMap(Vec3f(0,0,0), &path);
    addWidget(mm);
    Places *places = new Places(Vec3f(0,0,0));
    addWidget(places);
}

Agent *World::placeAgent(int place)
{
    if (!track)
        return NULL;
    int zp = place / 4;
    int xp = place % 2;
    int nsects = track->nSects;
    int backerSect = nsects - (zp + 1);
    int backSect = zp == 0 ? 0 : nsects - (zp);
    int frontSect = zp <= 1 ? 1 - zp : nsects - (zp - 1);
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

    return agent;
}

void World::makeAI()
{
    if (!track)
        return;
    AIManager &ai = AIManager::getInstance();
    int nAgents = numAgents();
    Agent *agent = placeAgent(nAgents);
    addAgent(agent);
    ai.control(agent);
    ai.controllers.back()->lane((nAgents + 1) % 2);
}


void World::makePlayer()
{
    if (!track)
        return;

    Agent *agent = placeAgent(numAgents());
    addAgent(agent);
    camera = Camera(THIRDPERSON, agent);
    Sound::getInstance().registerListener(&camera);
    PlayerController *p = new PlayerController(agent);
    Input::getInstance().controlPlayer(p);

    Speedometer *s = new Speedometer(Vec3f(0,0,0), agent);
    addWidget(s);
    LapCounter *lc = new LapCounter(Vec3f(0,0,0), agent);
    addWidget(lc);

}

void World::makeAgents()
{
    if (PlayerQty == 1) makePlayer();
    for (int i = 0; i < AIQty; i++)
    {
        makeAI();
    }
    AIManager &aim = AIManager::getInstance();
    for (unsigned int i = 0; i < wobjects.size(); i++)
    {
        if (!wobjects[i]->agent)
            continue;
        aim.agentsSorted.push_back(wobjects[i]->agent);
    }
}

void World::makeSkybox()
{
    SkyBoxInfo *sbox = new SkyBoxInfo("skybox/red/");
    Kinematic *k = new Kinematic(Vec3f(0,0,0), Vec3f(0,0,0), M_PI/2);
    PMoveable *geom = new PMoveable(k, 10, sbox);
    GObject *gobj = new GObject(sbox);
    WorldObject *wobj = new WorldObject(geom, gobj, NULL, NULL);
    addObject(wobj);
}

void World::setRunType(const string str){
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
        error->log(NETWORK, IMPORTANT,
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

std::ostream &operator<<(std::ostream &os, const WorldObject &wo)
{
    os << "pos: " << ((WorldObject) wo).getPos() << " "
       << "pobject: " << wo.pobject << " "
       << "gobject: " << wo.gobject << " "
       << "sobject: " << wo.sobject << " "
       << "agent: " << wo.agent << endl;
    return os;
}
