#ifndef AI_H
#define AI_H

#include <vector>
#include <deque>
#include "allclasses.h"
#include "Utilities/vec3f.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
  public:
    float                totalLength; /* !< total distance along path */
    unsigned int         index;     /* !< the first knot we haven't passed */
    std::vector<Vec3f>   knots;     /* !< knots defining the path */
    std::vector<float>   precision; /* !< how closely we want to follow the knots */
    Path ();
    Path (std::vector<Vec3f>);
    ~Path();

    void next();

    Vec3f closestPoint(Vec3f point);/*!< return point on path closest to point*/
    float pointToDist(Vec3f point);/*!<return distance on path of point */
    Vec3f distToPoint(float dist);/*!<return point at distance along path */
    Path *PathToPath(Vec3f position, float urgency);
    void clear();
};

/* A target that needs to be avoided */
class Avoid
{
  public:
    Vec3f     pos;   /* !< the position that must be avoided */
    float     str;   /* !< how wide a berth we need to give this object */
    double    time;  /* !< the time the object was created */
    double    ttl;   /* !< the time to live of the Avoid */
    Avoid();
    Avoid(Vec3f &);
    Avoid(Vec3f &, float);
    ~Avoid();
};

class AIController
{
    /* cached AI data? paths? etc */
    bool wallTrapped; /*Can we not even turn due to a wall in front of us? */
  public:
    Vec3f target;
    Vec3f antiTarget;
    AIController(Agent *);

    bool seeObstacle;
    WorldObject *obstacle;
    Vec3f obstaclePos;
    Path                path;           /* !< the path we're on */
    std::deque<Avoid> obstacles;        /* !< targets to be avoided */
    Agent               *agent;

    void detectWalls();
    void avoid(Vec3f&); /* !< add a new point to be avoided */
    void lane(int);     /* !< load a lane as the path */

    SteerInfo seek(const Vec3f target, float slowRadius = 0, float targetRadius = 1);
    SteerInfo align(float target, float slowRadius = 0, float targetRadius = 0);
    /*!<try to point to tgt; returns angle to tgt */
    SteerInfo face(Vec3f target); /*!<face target */
    SteerInfo brake();
    SteerInfo smartGo(const Vec3f target);
    SteerInfo cruise(Path *path);
    SteerInfo avoidObstacle(); /*!< avoid obstacle detected by detectWalls() */
    SteerInfo cruise();      /* !< follow path at maximum possible speed */
    SteerInfo followPath(int tubeRadius); /*!< follow path using Reynolds path
                                               following algo */
    SteerInfo followCarrot(int stickLength);

    void run(); /* !< Give new steering information to the agent we control */
  private:
    Error *error;
};

class AIManager
{
    AIManager();
    ~AIManager();
    AIManager(const AIManager *);
    AIManager &operator=(const AIManager&);
    static AIManager _instance;
  public:
    std::vector<AIController *> controllers; /* !< active AI controllers */

    void control(Agent *); /* !< assume AI control of given agent */
    void release(Agent *); /* !< release AI control of given agent */
    void run(); /* !< Give new steering information to each agent under our control*/

    static AIManager &getInstance();
  private:
    Error *error;
};


#endif
