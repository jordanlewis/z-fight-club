#include "vec3f.h"
#include "path.h"

Path::Path() : index(0) {}

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
    pathTotal = 0;
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

void Path::computeDistances()
{
    float dist;
    distances.clear();
    for (unsigned int i = 0; i < knots.size(); i++)
    {
        dist = pointToDist(knots[i]);
        distances.push_back(dist);
    }
}

float Path::knotDist(unsigned int knot)
{
    return distances[knot];
}
