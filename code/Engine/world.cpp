#include "world.h"
#include "Graphics/polygon.h"
extern "C" {
    #include "Parser/track-parser.h"
}

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World World::_instance;

World::World()
{
    environment = std::vector<Polygon>();
    agents      = std::vector<Agent *>();
}

World::~World()
{
    FreeTrackData(track);
}

void World::registerAgent(Agent *agent)
{
    agents.push_back(agent);
}

void World::loadTrack(char *file)
{
    track = LoadTrackData(file);
    if (!track)
	; /* error */
}

World &World::getInstance() 
{
    return _instance;
}
