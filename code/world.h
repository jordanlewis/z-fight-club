#ifndef WORLD_H
#define WORLD_H

class World
{
    Agent &agent[]; /* the agents in the world */
    
  public:
    World(int nAgents);
}

#endif
