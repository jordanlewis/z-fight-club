#ifndef WORLD_H
#define WORLD_H

#include <ode/ode.h>

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
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<Agent> agents; /* the agents in the world */
};

#endif
