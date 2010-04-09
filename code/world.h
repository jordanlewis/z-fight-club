#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "agent.h"

class World
{
    std::vector<Agent::Agent> agents; /* the agents in the world */

  public:
    World();
};

#endif
