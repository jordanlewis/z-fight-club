#ifndef POLYGON_H
#define POLYGON_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include <vector>

class Polygon {
    //std::vector<Vec3f_t> vertices; /* Can't have a vector of arrays */
    //TextureObject Texture; /* The texture to be applied to the object */

    Vec3f_t position;    /* The position of the object in world space */
    Vec3f_t orientation; /* The orientation of the object, where
                                orientation[0] = pitch
                                orientation[1] = yaw
                                orientation[2] = roll
                         NOTE: We may decide to go with quaternions later
                         */
  public:
    void Render();
};

#endif
