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
	; /* error */
}

const TrackData_t *World::getTrack()
{
    return track;
}

World &World::getInstance() 
{
    return _instance;
}
