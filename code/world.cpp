#include "world.h"

float World::xMax = 1000; // XXX this probably will depend on tracks
float World::zMax = 1000; // XXX this too

World::World()
{
    environment = std::vector<Polygon>();
    agents = std::vector<Agent>();
}
