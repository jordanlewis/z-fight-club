#ifndef AI_H
#define AI_H

#include <vector>
#include "vector.h"
#include "agent.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
    std::vector<Vec3f_t> knots;     /* !< knots defining the path */
    std::vector<float>   precision; /* !< how closely we want to follow the knots */
  public:
    Path ();
    Path (std::vector<Vec3f_t>);
    ~Path();

    Path *PathToPath(Vec3f_t position, float urgency);
};

class AIController
{
    /* cached AI data? paths? etc */
  public:
    Agent *agent;
    void run(); /* !< Give new steering information to the agent we control */
    AIController(Agent &);
};

class AIManager
{
    std::vector<AIController *> controllers; /* !< active AI controllers */

  public:
    void control(Agent &); /* !< assume AI control of given agent */
    void release(Agent &); /* !< release AI control of given agent */
    void run(); /* !< Give new steering information to each agent under our control*/
};


#endif
