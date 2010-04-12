#include <vector>
#include <math.h>
#include <stdlib.h>
#include "ai.h"
#include "vector.h"
#include "agent.h"

void Seek (Kinematic *car, float maxAccel, Vec3f_t target, SteerInfo *steer)
{
    Vec3f_t diff;
    SubV3f(target, car->pos, diff);
    NormalizeV3f(diff);
    ScaleV3f(maxAccel, diff, diff);

    steer->acceleration = LengthV3f(diff);
    steer->rotation = 0;
}

void Align (Kinematic *car, float maxRotation, float target, SteerInfo *steer)
{
    float targetRadius = .01;
    float slowRadius = 1;
    float diff = target - car->orientation;

    steer->acceleration = 0;

    diff = fmodf(diff, M_PI_2);
    if (diff > M_PI)
        diff -= M_PI_2;
    else if (diff < M_PI)
        diff += M_PI_2;

    float diffSize = abs(diff);

    /* we're already aligned. */
    if (diffSize < targetRadius)
    {
        steer->rotation = 0;
    }
    /* Turn at max speed */
    else if (diffSize > slowRadius)
    {
        steer->rotation = maxRotation;
    }
    /* slow down toward the end */
    else
    {
        steer->rotation = maxRotation * diffSize / slowRadius;
        steer->rotation *= diff / diffSize;
    }
}

AIController::AIController(Agent &agent)
{
    this->agent = &agent;
}

void AIController::run()
{

}

void AIManager::control(Agent &agent)
{
    AIController *newController = new AIController(agent);
    controllers.push_back(newController);
}

void AIManager::release(Agent &agent)
{
    /* is there a less ugly way of doing an iterator loop like this? */
    for (std::vector<AIController*>::iterator it = controllers.begin();
         it != controllers.end(); ++it)
    {
        /* Look through our controller array, and remove the controller that
         * has the agent with the ID of the input agent */
        if ((*it)->agent->id == agent.id)
        {
            controllers.erase(it);
            return;
        }
    }
}

void AIManager::run()
{
    for (unsigned int i = 0; i < controllers.size(); i++)
    {
        controllers[i]->run();
    }
}
