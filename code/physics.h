#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector.h"
#include "world.h"

namespace physics
{
    void simulate(World *world, float dt); /* step the world forward by dt. */
}

#endif
