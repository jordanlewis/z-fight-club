#ifndef AI_H
#define AI_H

#include <vector>
#include "vector.h"

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

#endif
