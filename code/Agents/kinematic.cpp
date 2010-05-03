#include "kinematic.h"

// ---------- Stream Output ----------
std::ostream &operator<<(std::ostream &os, const Kinematic &k)
{
    os << "Position: " << k.pos << std::endl;
    os << "Velocity: " << k.vel << std::endl;
    return os;
}
Kinematic::Kinematic(Vec3f pos, Vec3f vel, float orientation)
                    : pos(pos), vel(vel), orientation(orientation),
                      orientation_v(Vec3f(0,0,0))
{
}

float Kinematic::forwardSpeed() const
{
    return vel.dot(orientation_v);
}
