#include <vector>
#include <queue>
#include <math.h>
#include <stdlib.h>
#include "ai.h"
#include "Utilities/vec3f.h"
#include "Engine/input.h"
#include "agent.h"
#include "Engine/world.h"
#include "Utilities/error.h"

Path::Path() {}

AIManager AIManager::_instance;

Path::~Path() {}

void AIController::seek(const Vec3f target)
{
    Vec3f diff;
    SteerInfo s;
    Kinematic k = agent->getKinematic();
    diff = target - agent->kinematic.pos;

    align(atan2(diff[0], diff[2]));
    diff.normalize();

    diff *= agent->getMaxAccel();

    s = agent->getSteering();
    s.acceleration = diff.length();

    agent->setSteering(s);
}

void AIController::align(float target)
{
    float targetRadius = .01;
    float slowRadius = .01;
    float diff;
    SteerInfo s;
    Kinematic k = agent->getKinematic();
    diff = target - k.orientation;

    s.acceleration = 0;

    diff = fmodf(diff, 2 * M_PI);
    if (diff > M_PI)
        diff -= 2 * M_PI;
    else if (diff < -M_PI)
        diff += 2 * M_PI;

    float diffSize = abs(diff);

    /* we're already aligned. */
    if (diffSize < targetRadius)
    {
        s.rotation = 0;
    }
    /* Turn at max speed */
    else //if (diffSize > slowRadius)
    {
        s.rotation = agent->maxRotate;
        if (diff < 0)
            s.rotation *= -1;
    }
    /* slow down toward the end */
    /*
    else
    {
        s.rotation = agent->maxRotate * diffSize / slowRadius;
        s.rotation *= diff / diffSize;
    }
    */
    agent->setSteering(s);
}

AIController::AIController(Agent& agent)
{
    path = Path();
    this->agent = &agent;
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
    if ((path.knots.back() - agent->kinematic.pos).length() < path.precision.back()) {
	path.knots.pop();
	path.precision.pop();
    }

    seek(path.knots.front());
}

void AIController::run()
{
    cruise();
    /* Test target - we'll change this function to do more interesting things
     * once we get a better AI test architecture running. */
    /* Vec3f tgt = Input::getInstance().getPlayerController().getAgent().kinematic.pos;
    seek(tgt); */
}

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

AIManager &AIManager::getInstance()
{
    return _instance;
}
