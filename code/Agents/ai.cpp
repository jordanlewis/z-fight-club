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
#include "Parser/track-parser.h"

#define ARC_RESOLUTION 20
#define DEFAULT_PRECISION 2.0f

Path::Path() {}

AIManager AIManager::_instance;

Path::~Path() {}

void Path::clear()
{
    knots.clear();
    precision.clear();
}

void AIController::seek(const Vec3f target)
{
    Vec3f diff;
    SteerInfo s;
    Kinematic k = agent->getKinematic();
    diff = target - agent->kinematic.pos;

    align(atan2(diff[0], diff[2]));
    diff.normalize();

    diff *= (agent->getMaxAccel() / 20);

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

void AIController::lane(int laneIndex)
{
    int i, j;
    Error error = Error::getInstance();
    World &world = World::getInstance();

    /* check if this is a valid lane */
    if (laneIndex >= world.track->nLanes) {
	error.log(AI, IMPORTANT, "AI: asked to join a lane index out of range\n");
	return;
    }

    /* we're going to specify an entirely new path */
    path.clear();

    Lane_t lane = world.track->lanes[laneIndex];
    for (i = 0; i < lane.nSegs; i++) {
	path.knots.push_back(Vec3f(world.track->verts[lane.segs[i].start]));
	path.precision.push_back(DEFAULT_PRECISION); /* XXX doing a default value for now */
	if (lane.segs[i].kind == ARC_SEGMENT) {
	    for (j = 0; j < ARC_RESOLUTION; j++) {
		path.knots.push_back(slerp(Vec3f(world.track->verts[lane.segs[i].start]), Vec3f(world.track->verts[lane.segs[i].end]), (float) j / (float) ARC_RESOLUTION));
		path.precision.push_back(DEFAULT_PRECISION); /* XXX */
	    }
	}
	/* if we're on the last segment we need to put the end vertex on too */
	if (i == (lane.nSegs - 1)) {
	    path.knots.push_back(Vec3f(world.track->verts[lane.segs[i].end]));
	    path.precision.push_back(DEFAULT_PRECISION); /* XXX doing a default value for now */
	}
    }
}

void AIController::cruise()
{
    SteerInfo steerInfo;
    if ((path.knots.front() - agent->kinematic.pos).length() < path.precision.front()) {
	path.knots.pop_front();
	path.precision.pop_front();
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
