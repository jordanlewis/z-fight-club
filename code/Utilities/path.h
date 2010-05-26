#ifndef PATH_H
#define PATH_H

#include "allclasses.h"

/* \brief Class Path
 * \brief a path for an agent to follow
 */
class Path
{
    std::vector<float>   distances; /* !< pointToDist of knots, precomputed */
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

    void computeDistances(); /*!< populate distances vector; totalLength */

    float knotDist(unsigned int knot);

    Path *PathToPath(Vec3f position, float urgency);
    void clear();
};

#endif
