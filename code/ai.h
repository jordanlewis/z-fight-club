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
  public:
    int			    index;	/* !< the first knot we haven't passed */
    std::vector<Vec3f_t>    knots;	/* !< knots defining the path */
    std::vector<float>	    precision;	/* !< how closely we want to follow the knots */
    Path ();
    Path (std::vector<Vec3f_t>);
    ~Path();

    Path *PathToPath(Vec3f_t position, float urgency);
    const std::vector<Vec3f_t> *get_knots() const;
    const std::vector<float> *get_precision() const;
    void increase_index(int);
    int get_index() const;
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
