#ifndef AI_H
#define AI_H

#include <vector>
#include "Utilities/vec3f.h"
#include "agent.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
  public:
    int			    index;	/* !< the first knot we haven't passed */
    std::vector<Vec3f>    knots;	/* !< knots defining the path */
    std::vector<float>	    precision;	/* !< how closely we want to follow the knots */
    Path ();
    Path (std::vector<Vec3f>);
    ~Path();

    Path *PathToPath(Vec3f position, float urgency);
    const std::vector<Vec3f> *get_knots() const;
    const std::vector<float> *get_precision() const;
    void increase_index(int);
    int get_index() const;
};

class AIController
{
    /* cached AI data? paths? etc */
  public:
    Agent *agent;
    void seek(const Vec3f target);
    void align(float target);
    void cruise(Path *path);
    void run(); /* !< Give new steering information to the agent we control */
    AIController(Agent &);
};

class AIManager
{
    static AIManager _instance;
    AIManager();
    ~AIManager();
    AIManager(const AIManager&);
    AIManager &operator=(const AIManager&);

    std::vector<AIController *> controllers; /* !< active AI controllers */

  public:
    static AIManager &getInstance();
    void control(Agent &); /* !< assume AI control of given agent */
    void release(Agent &); /* !< release AI control of given agent */
    void run(); /* !< Give new steering information to each agent under our control*/
};


#endif
