#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector.h"
#include "world.h"
#include "agent.h"

class Physics
{
    World *world; /* our world */
    void updateAgentKinematic(Agent::Agent *agent, float dt);
  public:
    Physics(World *world); /* constructor; initialize with a world */
    void simulate(float dt); /* step the world forward by dt. */
};

#endif
