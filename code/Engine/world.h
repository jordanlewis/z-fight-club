#ifndef WORLD_H
#define WORLD_H

#include <ode/ode.h>

#include <vector>
#include "Agents/agent.h"
#include "Graphics/camera.h"
#include "Graphics/polygon.h"

class World
{
  private:
    std::vector<Polygon> environment; /* The Environmental geometry */

    // Prevent construction, destruction, copying, or assignment in singleton
    World();
    ~World();
    World(const World&);
    World& operator=(const World&);

    static World _instance;

  public:
    Camera camera;
    static float xMax;
    static float zMax;
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<Agent> agents; /* the agents in the world */

    void registerAgent(Agent &agent);

    static World &getInstance();

};

#endif
