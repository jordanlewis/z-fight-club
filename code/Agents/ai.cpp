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
#include "Utilities/defs.h"

#define ARC_RESOLUTION 20
#define DEFAULT_PRECISION 7.0f

Path::Path() {}

AIManager AIManager::_instance;

Path::~Path() {}

void Path::clear()
{
    knots.clear();
    precision.clear();
}

Avoid::Avoid() 
{
    time = GetTime();
    ttl = 3.0f;
}

Avoid::Avoid(Vec3f &pos)
{
    this->pos = pos;
    this->str = 1.0;
    time = GetTime();
    ttl = 3.0f;
}

Avoid::Avoid(Vec3f &pos, float str) 
{
    this->pos = pos;
    this->str = str;
    time = GetTime();
    ttl = 3.0f;
}

Avoid::~Avoid() {}

void AIController::seek(const Vec3f target, float slowRadius, float targetRadius)
{
    Vec3f dir;
    SteerInfo s;
    float distance;
    float targetSpeed;
    float maxAccel = agent->getMaxAccel();
    float maxSpeed = maxAccel * 3;
    Kinematic k = agent->getKinematic();
    dir = target - agent->kinematic.pos;
    distance = dir.length();
    dir.normalize();

    align(atan2(dir[0], dir[2]));

    s = agent->getSteering();
    if (distance < targetRadius)
    {
        s.acceleration = 0;
    }
    else if (distance > slowRadius)
    {
        /* Always executed without slowRadius parameter */
        s.acceleration = maxAccel;
    }
    else
    {
        /* If we're between our slowRadius and our targetRadius */
        targetSpeed = maxSpeed * distance / slowRadius;
        s.acceleration = (targetSpeed - k.vel.length()) / .1;
        if (s.acceleration > maxAccel)
            s.acceleration = maxAccel;
    }


    agent->setSteering(s);
}

float AIController::align(float target)
{
    float targetRadius = .01;
    float slowRadius = 0;
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
    else if (diffSize > slowRadius)
    {
        s.rotation = agent->maxRotate;
        if (diff < 0)
            s.rotation *= -1;
    }
    /* slow down toward the end */
    else
    {
        s.rotation = agent->maxRotate * diffSize / slowRadius;
        s.rotation *= diff / diffSize;
    }
    agent->setSteering(s);

    return diff;
}

void AIController::brake()
{
    Kinematic k = agent->getKinematic();
    SteerInfo s;
    float speed = k.orientation_v.dot(k.vel);
    if (speed > 0)
        s.acceleration = -agent->getMaxAccel();
    else if (speed < 0)
        s.acceleration = agent->getMaxAccel();
    agent->setSteering(s);
}

void AIController::smartGo(const Vec3f target)
{
    Vec3f dir;
    SteerInfo s;
    Error error = Error::getInstance();

    Kinematic k = agent->getKinematic();
    dir = target - agent->kinematic.pos;

    for (deque<Avoid>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
	/* nudge the dir to avoid obstacles */
	dir += (1.0f / pow((agent->kinematic.pos - it->pos).length(), 2.0f)) * it->str * (agent->kinematic.pos - it->pos);
    }

    float distance = dir.length();
    dir.normalize();

    float angle = abs(align(atan2(dir[0], dir[2])));
    /* Angle between 0 and pi */
    s = agent->getSteering();

    short go; /* 1 = accelerate, -1 = reverse acceleration, 0 = neither*/

    if (k.forwardSpeed() > 6)
    {
        if (angle < M_PI / 10)
        {
            error.log(AI, TRIVIAL, "AI: basic forward movement\n");
            go = 1;
        }
        else if (angle < 7 * M_PI / 8)
        {
            error.log(AI, TRIVIAL, "AI: sharp turn\n");
            go = -1;
        }
        else
        {
            error.log(AI, TRIVIAL, "AI: reverse movement\n");
            align(atan2(dir[0], dir[2]) + M_PI);
            s = agent->getSteering();
            go = -1;
        }
    }
    else
    {
        if (angle > 7 * M_PI / 8)
        {
            if (distance < 5)
            {
                error.log(AI, TRIVIAL, "AI: backing in\n");
                align(atan2(dir[0], dir[2]) + M_PI);
                s = agent->getSteering();
                go = -1;
            }
            else
            {
                go = 1;
                error.log(AI, TRIVIAL, "AI: turning around\n");
            }
        }
        else
        {
            error.log(AI, TRIVIAL, "AI: speed up\n");
            go = 1;
        }
    }
    s.acceleration = go * agent->getMaxAccel();
    agent->setSteering(s);
}

AIController::AIController(Agent *agent)
{
    path = Path();
    obstacles = std::deque<Avoid>(); 
    this->agent = agent;
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

    Vec3f us = agent->getKinematic().pos;
    int startSeg = -1;
    float dist;
    float bestDist = 100000;
    Vec3f bestMid, mid;
    int best = -1;
    TrackData_t *track = world.track;
    /* find the section the agent is currently in */
    for (i = 0; i < track->nSects; i++)
    {
        mid = lerp(track->verts[track->sects[i].edges[0].start],
                   track->verts[track->sects[i].edges[1].start], .5);
        mid = lerp(mid, track->verts[track->sects[i].edges[2].start],.5);
        mid = lerp(mid, track->verts[track->sects[i].edges[3].start],.5);

        dist = (mid - us).length();
        if (dist < bestDist)
        {
            bestDist = dist;
            best = i;
            bestMid = mid;
        }
    }
    /* now find the closest segment to that section */

    bestDist = 100000;
    best = -1;
    Lane_t lane = track->lanes[laneIndex];
    for (i = 0; i < lane.nSegs; i++)
    {
        dist = (Vec3f(track->verts[lane.segs[i].start]) - bestMid).length();
        if (dist < bestDist)
        {
            bestDist = dist;
            best = i;
        }
    }

    startSeg = best;

    for (i = startSeg + 1; i != startSeg;
         i = i < lane.nSegs ? i + 1 : 0) {
        path.knots.push_back(Vec3f(track->verts[lane.segs[i].start]));
        path.precision.push_back(DEFAULT_PRECISION); /* XXX doing a default value for now */
        if (lane.segs[i].kind == ARC_SEGMENT) {
	    Vec3f center = Vec3f(track->verts[lane.segs[i].center]);
	    Vec3f start =  Vec3f(track->verts[lane.segs[i].start]);
	    Vec3f end =  Vec3f(track->verts[lane.segs[i].end]);
	    // this way the theta should be != 180
	    center.x += lane.segs[i].end > lane.segs[i].start ? -.01 : .01;

            for (j = 1; j < ARC_RESOLUTION; j++) {
                path.knots.push_back(
                  slerp(start - center,
                        end - center,
                        (float) j / (float) ARC_RESOLUTION) +
                  center);
                path.precision.push_back(DEFAULT_PRECISION); /* XXX */
            }
        }
        /* if we're on the last segment we need to put the end vertex on too */
        if (i == (lane.nSegs - 1)) {
            path.knots.push_back(Vec3f(track->verts[lane.segs[i].end]));
            path.precision.push_back(DEFAULT_PRECISION); /* XXX doing a default value for now */
        }
    }
}

void AIController::avoid(Vec3f &pos)
{
    obstacles.push_back(Avoid(pos));
}

void AIController::cruise()
{
    double now = GetTime();
    for (deque<Avoid>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
	if((it->time + it->ttl) < now)
	    obstacles.erase(it);
    }

    SteerInfo steerInfo;
    if ((path.knots.front() - agent->kinematic.pos).length() < path.precision.front()) {
       path.knots.push_back(path.knots.front());
       path.knots.pop_front();
       path.precision.push_back(path.precision.front());
       path.precision.pop_front();
    }
    int tgtIdx = 0;
    /* Find a better node to seek to, ignoring walls */
    while ((path.knots.front() - path.knots[tgtIdx++]).length() <
           1.5 * path.precision.front());

    smartGo(path.knots[tgtIdx - 1]);
}

void AIController::run()
{
    cruise();
    /* Test target - we'll change this function to do more interesting things
     * once we get a better AI test architecture running. */
    /* Vec3f tgt = Input::getInstance().getPlayerController().getAgent().kinematic.pos;
    seek(tgt, 20, 2); */
}

AIManager::AIManager() {}

AIManager::~AIManager() {}

void AIManager::control(Agent *agent)
{
    AIController *newController = new AIController(agent);
    controllers.push_back(newController);
}

void AIManager::release(Agent *agent)
{
    /* is there a less ugly way of doing an iterator loop like this? */
    for (std::vector<AIController*>::iterator it = controllers.begin();
         it != controllers.end(); ++it)
    {
        /* Look through our controller array, and remove the controller that
         * has the agent with the ID of the input agent */
        if ((*it)->agent->id == agent->id)
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
