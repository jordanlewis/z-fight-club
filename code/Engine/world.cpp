#include "world.h"
#include "Graphics/polygon.h"

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World World::_instance;

World::World()
{
    environment = std::vector<Polygon>();
    agents      = std::vector<Agent>();
}

void World::registerAgent(Agent &agent)
{
    agents.push_back(agent);
}

World &getInstance() {
    return World::_instance;
}
