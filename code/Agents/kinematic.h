#ifndef KINEMATIC_H
#define KINEMATIC_H

#include "Utilities/vec3f.h"

class Kinematic
{
  public:
    Vec3f pos;
    Vec3f vel;
    Vec3f orientation_v;

    float orientation;
};

//! \brief Writes this kinematic to the given output stream.
//         This will be used by the analogous operator for agent
//         which will in turn be used for debugging.
std::ostream &operator<<(std::ostream&, const Kinematic&);
#endif
