#ifndef KINEMATIC_H
#define KINEMATIC_H

#include "Utilities/vec3f.h"

struct RPKinematic;

class Kinematic
{
  public:
    Vec3f pos;
    Vec3f vel;
    float orientation;

    Vec3f orientation_v;

    Kinematic(Vec3f pos=Vec3f(0,0,0), Vec3f vel=Vec3f(0,0,0),
              float orientation=0.0);
    float forwardSpeed() const; /*Return the speed along orientation_v */
    void hton(RPKinematic *payload);
    void ntoh(RPKinematic *payload);
};

//! \brief Writes this kinematic to the given output stream.
//         This will be used by the analogous operator for agent
//         which will in turn be used for debugging.
std::ostream &operator<<(std::ostream&, const Kinematic&);
#endif
