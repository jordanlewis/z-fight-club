#include <vector>
#include <queue>
#include <math.h>
#include <stdlib.h>
#include "ai.h"
#include "Utilities/vec3f.h"
#include "agent.h"
#include "Engine/world.h"
#include "Utilities/error.h"

Path::Path() {}

Path::~Path() {}

void seek (Kinematic *car, float maxAccel, const Vec3f target, SteerInfo *steer)
{
    Vec3f diff;
    diff = target - car->pos;
    diff.normalize();
    diff *= maxAccel;

    steer->acceleration = diff.length();
    steer->rotation = 0;
}

void align (Kinematic *car, float maxRotation, float target, SteerInfo *steer)
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

AIController::AIController(Agent& agent)
{
    path = Path();
    this->agent = agent;
}

void AIController::lane(int lane)
{
    Error error = Error::getInstance();
    World &world = World::getInstance();
    if (lane >= world.track->nLanes) {
	error.log(AI, IMPORTANT, "AI: asked to join a lane index out of range\n");
	return;
    }
}

void AIController::cruise()
{
    SteerInfo steerInfo;
    if ((path.knots.back() - agent.kinematic.pos).length() < path.precision.back()) {
	path.knots.pop();
	path.precision.pop();
    }

    seek(&agent.kinematic, agent.maxAccel, path.knots.front(), &steerInfo);
    agent.setSteering(steerInfo);
}

void AIController::run()
{
    cruise();
}

AIManager AIManager::_instance;

AIManager::AIManager() {}

AIManager::~AIManager() {}

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
        if ((*it)->agent.id == agent.id)
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

AIManager& AIManager::getInstance()
{
    return _instance;
}
