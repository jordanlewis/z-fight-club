#ifndef AI_H
#define AI_H

#include <vector>
#include <deque>
#include "allclasses.h"
#include "Utilities/vec3f.h"
#include "Utilities/path.h"
#include "Physics/collision.h"
#include "Engine/component.h"

/*! \class Avoid
 *  \brief A target that needs to be avoided i
 */
class Avoid
{
  public:
    Vec3f     pos;          /* !< the position that must be avoided */
    float     str;          /* !< how wide a berth we need to give this object */
    double    time;         /* !< the time the object was created */
    double    ttl;          /* !< the time to live of the Avoid */
    Avoid();                /* !< default constructor */
    Avoid(Vec3f &);         /* !< constructor that sets position */
    Avoid(Vec3f &, float);  /* !< constructor that sets position and str */
    ~Avoid();               /* !< destructor */
};

/*! \class AIController
 *  \brief Takes input from the world and sends driving instructions to a controlled racer
 */
class AIController
{
    /* cached AI data? paths? etc */
    bool wallTrapped;                   // !< Can we not even turn due to a wall in front of us?
    bool turnedAround;                  // !< Are we turned around?
    double lastShot;                    // !< last time we shot
  public:
    Vec3f target;                       // !< where we want to go
    Vec3f antiTarget;                   // !< a target to be avoided
    AIController(Agent /);

    bool seeObstacle;                   // !< do we see and obstacle
    CollContact obstacle;
    Path                path;           // !< the path we're on
    std::deque<Avoid> obstacles;        // !< targets to be avoided
    Agent               /agent;         // !< the agent being controlled

    void detectWalls();                 // !< checks for possible wall collisions and creates obstacles so that they are avoided
    void avoid(Vec3f&);                 // !< add a new point to be avoided
    void lane(int);                     // !< load a lane as the path

    SteerInfo seek(const Vec3f target, float slowRadius = 0, float targetRadius = 0);
    SteerInfo align(float target, float slowRadius = 0, float targetRadius = 0);
    SteerInfo face(Vec3f target);
    SteerInfo brake();                                                                  /* !< decrease speed as fast as possible */
    SteerInfo smartGo(const Vec3f target);
    SteerInfo cruise(Path *path);                                                       /* !< calling supporting functions to travel safely around the path */
    SteerInfo avoidObstacle();                                                          /* !< avoid obstacle detected by detectWalls() */
    SteerInfo cruise();                                                                 /* !< follow path at maximum possible speed */
    SteerInfo followPath(int tubeRadius);                                               /* !< follow path using Reynolds path following algo */
    SteerInfo followCarrot(int stickLength);                                            /* !< seek a point ahed on the path */

    void run();                                                                         /* !< Give new steering information to the agent we control */
  private:
    Error *error;                                                                       /* !< our error reporting class */
};

/*! \class AIManager  
 *  \brief singleton class for scheduling the AIControllers
 */
class AIManager : public Component
{
    AIManager();                                /* !< default constructor */
    ~AIManager();                               /* !< default destructor */
    AIManager(const AIManager *);               /* !< copy constructor */
    AIManager &operator=(const AIManager&);     /* !< comparison operator */
    static AIManager _instance;                 /* the singleton instance */
  public:
    std::vector<AIController *> controllers;    /* !< active AI controllers */
    std::vector<Agent *> agentsSorted;          /* !< Used for computing place info */

    AIController *control(Agent *);             /* !< assume AI control of given agent */
    void release(Agent *);                      /* !< release AI control of given agent */
    void run();                                 /* !< Give new steering information to each agent under our control*/

    static AIManager &getInstance();            /* !< return the singleton instance */
  private:
    Error *error;                               /* !< our error reporting instance */
};


#endif
