#include "kinematic.h"

// ---------- Stream Output ----------
std::ostream &operator<<(std::ostream &os, const Kinematic &k)
{
    os << "Position: " << k.pos << std::endl;
    os << "Velocity: " << k.vel << std::endl;
    return os;
}
