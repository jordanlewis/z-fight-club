#include <vector>
#include <queue>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "ai.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"
#include "Utilities/path.h"
#include "Engine/input.h"
#include "agent.h"
#include "Engine/world.h"
#include "Parser/track-parser.h"
#include "Utilities/defs.h"
#include "Physics/collision.h"

#define ARC_RESOLUTION 20
#define DEFAULT_PRECISION 7.0f

AIManager AIManager::_instance;

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

AIController::AIController(Agent *agent) :
    wallTrapped(false), seeObstacle(false), path(Path()),
    obstacles(std::deque<Avoid>()), agent(agent), error(&Error::getInstance())
{
}

SteerInfo AIController::seek(const Vec3f target, float slowRadius, float targetRadius)
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

    s = face(target);
    float r = s.rotation;

    s = agent->getSteering();
    s.rotation = r;
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

    return s;
}

SteerInfo AIController::face(Vec3f target)
{
    Vec3f dir = target - agent->getKinematic().pos;
    float angle = atan2(dir.x, dir.z);

    return align(angle);
}

SteerInfo AIController::align(float target, float slowRadius, float targetRadius)
{
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
    if (diffSize <= targetRadius)
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

    return s;
}

SteerInfo AIController::brake()
{
    Kinematic k = agent->getKinematic();
    SteerInfo s;
    float speed = k.orientation_v.dot(k.vel);
    if (speed > 0)
        s.acceleration = -agent->getMaxAccel();
    else if (speed < 0)
        s.acceleration = agent->getMaxAccel();

    return s;
}

SteerInfo AIController::smartGo(const Vec3f target)
{
    Vec3f dir;
    SteerInfo s;

    Kinematic k = agent->getKinematic();
    dir = target - k.pos;

    for (deque<Avoid>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
        /* nudge the dir to avoid obstacles */
        dir += (1.0f / pow((k.pos - it->pos).length(), 2.0f)) * it->str * (k.pos - it->pos);
    }

    float distance = dir.length();
    dir.normalize();

    s = align(atan2(dir[0], dir[2]));

    float angle = acos(dir.dot(k.orientation_v));

    short go; /* 1 = accelerate, -1 = reverse acceleration, 0 = neither*/
    bool overrideTurn = false;
    short turn = 0; /* 1 = left, -1 = right, 0 = neither */

    if (wallTrapped)
    {
        /* We're trapped by a wall. Reverse and turn away from the wall */
        go = -1;
    }
    else
    {
        if (angle < M_PI / 4)
            go = 1;
        else if (angle < 3 * M_PI / 4)
            go = .8;
        else if (angle <= 2 * M_PI)
        {
            if (distance < 5)
            {
                align(atan2(dir[0], dir[2]) + M_PI);
                s = agent->getSteering();
            }
            go = -1;
        }
    }
    s.acceleration = go * agent->getMaxAccel();
    if (overrideTurn)
    {
        s.rotation = turn * agent->maxRotate;
    }

    return s;
}

void AIController::lane(int laneIndex)
{
    int i, j;
    World &world = World::getInstance();

    /* check if this is a valid lane */
    if (laneIndex >= world.track->nLanes) {
        error->log(AI, IMPORTANT, "AI: asked to join a lane index out of range\n");
        return;
    }

    /* we're going to specify an entirely new path */
    path.clear();

    Vec3f us = agent->getKinematic().pos;
    float dist;
    float bestDist = FLT_MAX;
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

    bestDist = FLT_MAX;
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

    path.index = best;

    for (i = 0; i < lane.nSegs; i++) {
        int seg = i;

        path.knots.push_back(Vec3f(track->verts[lane.segs[seg].start]));
        path.precision.push_back(DEFAULT_PRECISION); /* XXX doing a default value for now */
        if (lane.segs[seg].kind == ARC_SEGMENT) {
            Vec3f center = Vec3f(track->verts[lane.segs[seg].center]);
            Vec3f start =  Vec3f(track->verts[lane.segs[seg].start]);
            Vec3f end =  Vec3f(track->verts[lane.segs[seg].end]);
            // this way the theta should be != 180
            center.x += lane.segs[seg].end > lane.segs[seg].start ? -.01 : .01;

            for (j = 1; j < ARC_RESOLUTION; j++) {
                path.knots.push_back(
                  slerp(start - center,
                        end - center,
                        (float) j / (float) ARC_RESOLUTION) +
                  center);
                path.precision.push_back(DEFAULT_PRECISION); /* XXX */
            }
        }
    }
    path.computeDistances();
}

void AIController::avoid(Vec3f &pos)
{
    obstacles.push_back(Avoid(pos));
}

void AIController::detectWalls()
{
    const Kinematic k = agent->getKinematic();
    wallTrapped = false;
    seeObstacle = false;
    if (k.vel.length() == 0)
        return;

    float frontLength = 20; /* How far to cast rays */
    float diagLength = 1;
    CollQuery queryl, queryr, query;

    /* ray starts at the front of agent */
    const Vec3f start = k.pos + k.orientation_v.unit() * (.01 + agent->depth / 2);

    /* get start points at left and right corners of agent, going out at angle*/
    Vec3f perp = k.orientation_v.perp(Vec3f(0,1,0));
    Vec3f startl = start - perp * agent->width / 2;
    Vec3f startr = start + perp * agent->width / 2;

    Vec3f langle = slerp(-perp, k.orientation_v, .75);
    Vec3f rangle = slerp(perp, k.orientation_v, .75);

    /* Cast rays from all three start points */
    rayCast(&startl, &langle, diagLength, &queryl);
    rayCast(&startr, &rangle, diagLength, &queryr);
    rayCast(&start, &k.orientation_v, frontLength, &query);

    CollContact *contact;
    CollContact *closest = NULL;

    Vec3f pos;
    float bestDist = FLT_MAX;

    /* Check for wall trapped-ness: if 2 out of 3 rays' distance to collision
     * is close to the same small value, then we're probably directly facing
     * and stuck against a wall.
     */
    CollQuery *q;
    const Vec3f *s;
    for (int i = 0; i < 3; i++)
    {
        switch (i)
        {
            case 0: q = &query;  s = &start;  break;
            case 1: q = &queryl; s = &startl; break;
            case 2: q = &queryr; s = &startr; break;
        }
        if (q->contacts.size() == 0)
            continue;

        contact = &(q->contacts.front());

        if (contact->obj == NULL || contact->obj == agent->worldObject ||
            contact->obj == World::getInstance().floorObj)
            continue;

        if (contact->obj->agent == NULL && i == 0)
        {
            /* Collided with static environment. check normal and distance
             * to see if we might be stuck. */
            if (contact->distance < agent->depth * 2 &&
                abs(contact->normal.unit().dot(k.orientation_v.unit())) > .9)
            {
                antiTarget = contact->position;
                wallTrapped = true;
                return;
            }
        }

        /* Get closest contact for non-wallstuck cases */
        if (contact->distance < bestDist)
        {
            closest = contact;
        }
    }

    if (closest != NULL)
    {
        seeObstacle = true;
        obstacle.distance = contact->distance;
        obstacle.normal = contact->normal;
        obstacle.position = contact->position;
        obstacle.obj = contact->obj;
    }
}

SteerInfo AIController::avoidObstacle()
{
    SteerInfo s;
    if (seeObstacle == false || turnedAround == true)
        return s;

    Vec3f q, v, avoidDir;
    float a,b,c,discriminant,sqrtdisc,post,negt,hitTime;

    /* Determine angle to obstacle. If between 0 and pi/2, turn right. If
        * between pi/2 and pi, turn left. Else its behind us. */
    const Kinematic k = agent->getKinematic();

    antiTarget = obstacle.position;

    hitTime = FLT_MAX;
    if (obstacle.obj->agent)
    {
        /* determine next collision point given linear motion */
        v = k.vel - obstacle.obj->agent->getKinematic().vel;
        q = k.pos - obstacle.obj->agent->getKinematic().pos;
        a = v.dot(v);
        b = q.dot(v) * 2;
        c = q.dot(q) - 4 * (agent->width/2) * (agent->width/2);
        discriminant = (b * b) - (4 * a * c);

        if (discriminant >= 0)
        {
            sqrtdisc = sqrt(discriminant);
            post = (-b + sqrtdisc) / (2 * a);
            negt = (-b - sqrtdisc) / (2 * a);

            if (post > 0 && negt > 0)
            {
                hitTime = post < negt ? post : negt;
            }
        }
        if (hitTime < 2)
        {
            s = face(obstacle.obj->agent->getKinematic().orientation_v.perp() +
                     k.pos);
        }

    }
    else
    {
        /* static geometry: try to align to the normal of the surface */
        s = face(k.pos + obstacle.normal);
    }
    return s;
}

SteerInfo AIController::cruise()
{
    double now = GetTime();
    for (deque<Avoid>::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
        if((it->time + it->ttl) < now)
            obstacles.erase(it);
    }

    if ((path.knots[path.index] -
         agent->kinematic.pos).length() < path.precision[path.index])
    {
        path.next();
    }

    int tgtIdx = path.index;
    /* Find a better node to seek to, ignoring walls */
    while ((path.knots[path.index] -
            path.knots[tgtIdx++ % path.knots.size()]).length() <
           1.5 * path.precision[path.index]);

    return smartGo(path.knots[tgtIdx]);
}

SteerInfo AIController::followPath(int tubeRadius)
{
    SteerInfo s;
    Kinematic k = agent->getKinematic();
    Vec3f futureGuess, guessOnPath, curOnPath;
    float guessError, curDist, guessDist;

    turnedAround = false;

    if (k.vel.length() <= 1)
        futureGuess = k.pos + k.orientation_v * 5;
    else
        futureGuess = k.pos + k.vel;

    guessOnPath = path.closestPoint(futureGuess);
    curOnPath   = path.closestPoint(k.pos);
    guessDist = path.pointToDist(guessOnPath);
    curDist = fmodf(path.pointToDist(curOnPath), path.totalLength);

    float forwardDist = path.pointToDist(k.pos + k.orientation_v * 4);
    if (forwardDist <= curDist &&
        abs(forwardDist - curDist) < (k.orientation_v * 8).length())
    {
        turnedAround = true;
        // we're turned around
        guessOnPath = path.distToPoint(curDist + 10);
    }

    guessError = (guessOnPath - futureGuess).length();

    if (guessError > tubeRadius)
        target = guessOnPath + guessOnPath - futureGuess;
    else
        target = futureGuess;

    return smartGo(target);

}

SteerInfo AIController::followCarrot(int stickLength)
{
    SteerInfo s;
    Kinematic k = agent->getKinematic();

    Vec3f closest = path.closestPoint(k.pos);

    target = path.distToPoint(path.pointToDist(closest) + stickLength);

    return smartGo(target);

}

void AIController::run()
{
    detectWalls();
    SteerInfo s = followPath(1);
    SteerInfo obst = avoidObstacle();
    if (obst.rotation != 0)
        s.rotation = obst.rotation;
    agent->setSteering(s);
}

AIManager::AIManager() :
    Component(), error(&Error::getInstance())
{
    frequency = 50;
}

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
        if ((*it)->agent == agent)
        {
            controllers.erase(it);
            return;
        }
    }
}

static bool agentCmp(Agent *a, Agent *b)
{
    if (a->lapCounter != b->lapCounter)
        return b->lapCounter < a->lapCounter;

    return b->pathDistance < a->pathDistance;
}

void AIManager::run()
{
    if (!start())
        return;
    World &world = World::getInstance();
    for (unsigned int i = 0; i < controllers.size(); i++)
    {
        controllers[i]->run();
    }


    /* Update agent lap count and position */
    float agentDist, knotDist;
    Agent *agent;
    Path *path = &world.path;
    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        agent = world.wobjects[i]->agent;
        if (NULL == agent)
            continue;
        agentDist = path->pointToDist(agent->getKinematic().pos);
        agent->pathDistance = agentDist;
        knotDist  = path->knotDist(agent->pathPosition);
        if (agentDist > knotDist &&
            agentDist < knotDist + path->precision[agent->pathPosition])
        {
            if (++agent->pathPosition >= path->knots.size())
            {
                agent->pathPosition = 0;
            }
            else if (agent->pathPosition == 1)
            {
                agent->lapCounter++;
                agent->nextLap();
            }
        }
    }

    sort(agentsSorted.begin(), agentsSorted.end(), agentCmp);
    finish();
}

AIManager &AIManager::getInstance()
{
    return _instance;
}
