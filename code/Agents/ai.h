#ifndef AI_H
#define AI_H

#include <vector>
#include <queue>
#include "Utilities/vec3f.h"
#include "agent.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
  public:
    int			    	index;		/* !< the first knot we haven't passed */
    std::queue<Vec3f> 		knots;		/* !< knots defining the path */
    std::queue<float>	    	precision;	/* !< how closely we want to follow the knots */
    Path ();
    Path (std::vector<Vec3f>);
    ~Path();

    Path *PathToPath(Vec3f position, float urgency);
};

class AIController
{
    /* cached AI data? paths? etc */
  public:
    Path path;	/* !< the path we're on */
    Agent agent;
    AIController(Agent &);
    void lane(int);	/* !< load a lane as the path */
    void cruise(); 	/* !< follow path at maximum possible speed */
    void run(); 	/* !< Give new steering information to the agent we control */
};

class AIManager
{
  public:
    std::vector<AIController *> controllers; /* !< active AI controllers */

    static AIManager _instance;

    AIManager();
    ~AIManager();
    void control(Agent &); /* !< assume AI control of given agent */
    void release(Agent &); /* !< release AI control of given agent */
    void run(); /* !< Give new steering information to each agent under our control*/

    static AIManager &getInstance();
};


#endif
