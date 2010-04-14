#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "agent.h"
#include "camera.h"
#include "polygon.h"

class World
{
    std::vector<Polygon> environment; /* The Environmental geometry */

    Camera camera;

  public:
    static float xMax;
    static float zMax;
    std::vector<Agent> agents; /* the agents in the world */
    World();
};

#endif
