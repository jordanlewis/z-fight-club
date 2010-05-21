#include <vector>
#include <queue>
#include <math.h>
#include <stdlib.h>
#include "ai.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"
#include "Engine/input.h"
#include "agent.h"
#include "Engine/world.h"
#include "Parser/track-parser.h"
#include "Utilities/defs.h"
#include "Physics/collision.h"

#define ARC_RESOLUTION 20
#define DEFAULT_PRECISION 7.0f

Path::Path() : index(0) {}

AIManager AIManager::_instance;

Path::~Path() {}

void Path::clear()
{
    knots.clear();
    precision.clear();
}

void Path::next()
{
    if (++index >= knots.size())
        index = 0;
}

static Vec3f closestPointOnSegment(Vec3f point, Vec3f enda, Vec3f endb)
{
    float proj;
    Vec3f diff, seg, pnorm, closest;

    diff = point - enda;
    seg = endb - enda;
    pnorm = seg / seg.length();
    proj = pnorm.dot(diff);
    if (proj > seg.length())
        closest = endb;
    else if (proj < 0)
        closest = enda;
    else
        closest = pnorm * proj + enda;

    return closest;
}

Vec3f Path::closestPoint(Vec3f point)
{
    float dist, bestDist = 10000;
    Vec3f closest, bestClosest, next;
    for (unsigned int i = 0; i < knots.size(); i++)
    {
        /* Compute distance from point to line segment knot -> nextKnot */
        next = knots[(i + 1) % knots.size()];

        closest = closestPointOnSegment(point, knots[i], next);

        dist = (point - closest).length();

        if (dist < bestDist)
        {
            bestDist = dist;
            bestClosest = closest;
        }
    }
    return bestClosest;
}

float Path::pointToDist(Vec3f point)
{
    Vec3f closest, next;
    float len, dist, bestDist, pathTotal, finalDist;
    bestDist = 1000000;
    for (unsigned int i = 0; i < knots.size(); i++)
    {
        next = knots[(i + 1) % knots.size()];

        len = (knots[i] - next).length();
        closest = closestPointOnSegment(point, knots[i], next);
        dist = (point - closest).length();

        /* The smallest distance indicates we're on the closest segment to
         * the point. So write the return value as the total value plus the
         * segment-point distance here, and we'll return it if it is indeed
         * the smallest distnace.*/
        if (dist < bestDist)
        {
            bestDist = dist;
            finalDist = pathTotal + (closest - knots[i]).length();
        }

        pathTotal += len;
    }

    return finalDist;
}

Vec3f Path::distToPoint(float dist)
{
    dist = fmodf(dist, totalLength);

    float total, len;
    Vec3f next, point;
    for (unsigned int i = 0; i < knots.size(); i++)
    {
        next = knots[(i + 1) % knots.size()];

        len = (knots[i] - next).length();
        total += len;
        if (total > dist)
        {
            point = lerp(knots[i], next, 1 - (total - dist) / len);
            break;
        }
    }
    return point;
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
        else
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

    path.totalLength = 0;
    for (unsigned int i = 0; i < path.knots.size() - 1; i++)
    {
        path.totalLength += (path.knots[i] - path.knots[i + 1]).length();
    }
    path.totalLength += (path.knots[0] - path.knots[path.knots.size() - 1]).length();

}

void AIController::avoid(Vec3f &pos)
{
    obstacles.push_back(Avoid(pos));
}

void AIController::detectWalls()
{
    const Kinematic k = agent->getKinematic();
    wallTrapped = false;
    if (k.vel.length() == 0)
        return;

    float length = 10; /* How far to cast rays */
    CollQuery queryl, queryr, query;

    /* ray starts at the front of agent */
    const Vec3f start = k.pos + k.orientation_v.unit() * (.01 + agent->depth / 2);

    /* get start points at left and right corners of agent */
    Vec3f perp = k.orientation_v.perp(Vec3f(0,1,0));
    Vec3f startl = start - perp * agent->width / 2;
    Vec3f startr = start + perp * agent->width / 2;

    /* Cast rays from all three start points */
    rayCast(&startl, &k.orientation_v, length, &queryl);
    rayCast(&startr, &k.orientation_v, length, &queryr);
    rayCast(&start, &k.orientation_v, length, &query);

    CollContact contact;

    WorldObject *closest = NULL;
    Vec3f pos;
    float bestDist = 10000;

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

        contact = *(q->contacts.begin());

        if (contact.obj == NULL || contact.obj == agent->worldObject)
            continue;

        if (contact.obj->agent == NULL)
        {
            /* Collided with static environment. check normal and distance
             * to see if we might be stuck. */
            if (contact.distance < agent->depth * 2 &&
                abs(contact.normal.unit().dot(k.orientation_v.unit())) > .9)
            {
                wallTrapped = true;
                return;
            }
        }

        /* Get closest contact for non-wallstuck cases */
        if (contact.distance < bestDist)
        {
            closest = contact.obj;
            pos = contact.position;
        }
    }

    if (closest != NULL)
    {
        seeObstacle = true;
        obstaclePos = pos;
        obstacle = closest;
    }
    else
    {
        seeObstacle = false;
    }
}

SteerInfo AIController::avoidObstacle()
{
    SteerInfo s;
    if (seeObstacle == false)
        return s;

    Vec3f q, v, avoidDir;
    float a,b,c,discriminant,sqrtdisc,post,negt,hitTime, obstAngle;

    /* Determine angle to obstacle. If between 0 and pi/2, turn right. If
        * between pi/2 and pi, turn left. Else its behind us. */
    const Kinematic k = agent->getKinematic();

    avoidDir = obstaclePos - k.pos;
    obstAngle = atan2(avoidDir.x, avoidDir.z) - k.orientation;

    obstAngle = fmodf(obstAngle, 2 * M_PI);
    if (obstAngle > M_PI)
        obstAngle -= 2 * M_PI;
    else if (obstAngle < -M_PI)
        obstAngle += 2 * M_PI;

    hitTime = 1000;
    if (obstacle->agent)
    {
        /* determine next collision point given linear motion */
        v = k.vel - obstacle->agent->getKinematic().vel;
        q = k.pos - obstacle->agent->getKinematic().pos;
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

    }
    s.rotation = 0;
    if (!obstacle->agent || hitTime < 1) /* assuming static geometry */
    {
        antiTarget = obstacle->getPos();
        if (obstAngle > 0)
            s.rotation = -agent->maxRotate;
        else if (obstAngle < 0)
            s.rotation = agent->maxRotate;
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


    if (k.vel.length() <= 1)
        futureGuess = k.pos + k.orientation_v * 5;
    else
        futureGuess = k.pos + k.vel;

    guessOnPath = path.closestPoint(futureGuess);
    curOnPath   = path.closestPoint(k.pos);
    guessDist = path.pointToDist(guessOnPath);
    curDist = fmodf(path.pointToDist(curOnPath), path.totalLength);

    if (guessDist < curDist)
    {
        // we're turned around
        guessOnPath = path.distToPoint(curDist + 10);
    }

    guessError = (guessOnPath - futureGuess).length();

    if (guessError > tubeRadius)
        target = guessOnPath;
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
    SteerInfo s = cruise();
    SteerInfo obst = avoidObstacle();
    if (obst.rotation != 0)
        s.rotation = obst.rotation;
    agent->setSteering(s);
}

AIManager::AIManager() :
    error(&Error::getInstance())
{}

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

void AIManager::run()
{
    error->pin(P_AI);
    for (unsigned int i = 0; i < controllers.size(); i++)
    {
        controllers[i]->run();
    }
    error->pout(P_AI);
}

AIManager &AIManager::getInstance()
{
    return _instance;
}
