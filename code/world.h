#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "agent.h"
#include "camera.h"
#include "polygon.h"

class World
{  
    std::vector<Polygon> environment; /* The Environmental geometry */
    std::vector<Agent::Agent> agents; /* the agents in the world */

    Graphics::Camera camera;

  public:
    World();
};

#endif
