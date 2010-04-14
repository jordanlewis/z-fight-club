#include <math.h>
#include "physics.h"
#include "vector.h"
#include "world.h"
#include "agent.h"

void Physics::updateAgentKinematic(Agent::Agent *agent, float dt)
{
    /* Position += velocity * time */
    ScaledAddV3f(agent->pos.orig, dt, agent->pos.dir, agent->pos.orig);

    /* Orientation += rotation * time */
    agent->orientation += agent->steerInfo.rotation * dt;
    agent->orientation = fmodf(agent->orientation, 2 * M_PI);
    /* Update velocity vector so it lies along orientation */
    float speed = LengthV3f(agent->pos.dir);
    agent->pos.dir[0] = sin(agent->orientation) * speed;
    agent->pos.dir[2] = cos(agent->orientation) * speed;

    /* Velocity += acceleration * time */
    ScaledAddV3f(agent->pos.dir, agent->steerInfo.acceleration * dt,
                 agent->pos.dir, agent->pos.dir);

}

void Physics::simulate(float dt)
{
    for (unsigned int i = 0; i < world->agents.size(); i++)
    {
        updateAgentKinematic(&world->agents[i], dt);
    }
}

Physics::Physics(World *world)
{
    this->world = world;
}
