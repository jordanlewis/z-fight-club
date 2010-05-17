#ifndef AI_H
#define AI_H

#include <vector>
#include <deque>
#include "Utilities/vec3f.h"
#include "Utilities/error.h"
#include "agent.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
  public:
    int                 index;     /* !< the first knot we haven't passed */
    std::deque<Vec3f>   knots;     /* !< knots defining the path */
    std::deque<float>   precision; /* !< how closely we want to follow the knots */
    Path ();
    Path (std::vector<Vec3f>);
    ~Path();

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
    bool seeObstacle;
  public:
    Vec3f obstacle;
    Path                path;           /* !< the path we're on */
    std::deque<Avoid> obstacles;        /* !< targets to be avoided */
    Agent               *agent;
    void seek(const Vec3f target, float slowRadius = 0, float targetRadius = 1);
    float align(float target); /*!<try to point to tgt; returns angle to tgt */
    void brake();
    void smartGo(const Vec3f target);
    void detectWalls();
    void cruise(Path *path);
    AIController(Agent *);
    void lane(int);     /* !< load a lane as the path */
    void avoid(Vec3f&); /* !< add a new point to be avoided */
    void avoidObstacle(); /*!< avoid obstacle detected by detectWalls() */
    void cruise();      /* !< follow path at maximum possible speed */
    void run();         /* !< Give new steering information to the agent we control */
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
