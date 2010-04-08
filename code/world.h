#ifndef WORLD_H
#define WORLD_H

#include "agent.h"

class World
{
    class Agent *agent[]; /* the agents in the world */
    
  public:
    World(int nAgents);
};

#endif
