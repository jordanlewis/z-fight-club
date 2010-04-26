#include <vector>
#include <cmath>
#include <cstdlib>
#include "ai.h"
#include "Utilities/vec3f.h"
#include "agent.h"

AIManager AIManager::_instance;

const std::vector<Vec3f>* Path::get_knots() const
{
    return &(this->knots);
}

const std::vector<float>* Path::get_precision() const
{
    return &(this->precision);
}

void Path::increase_index(int n)
{
    this->index += n;
    return;
}

int Path::get_index() const
{
    return this->index;
}



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

void AIController::cruise (Path *path)
{
    Vec3f dist;
    Kinematic k = agent->getKinematic();
    Vec3f a = path->get_knots()->front();
    dist = a - k.pos;
    if (dist.length() < path->get_precision()->front())
	path->increase_index(1);
    
    seek((*path->get_knots())[path->get_index()]);
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

AIManager &AIManager::getInstance()
{
    return _instance;
}

AIManager::AIManager()
{
}

AIManager::~AIManager()
{
}
