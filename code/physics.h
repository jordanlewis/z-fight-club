#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector.h"

namespace physics
{
    class collision {
	float t; /* How far along the given ray our collision occurs */
	int coll_id; /* Indentifier for the object that we collided with */
    }


    void simulate(); /* step the world forward. */
    class collision ray_collision(Ray_ft ray, float len); /* Test if collision
							 will occur along this
							 ray.  If so, return
							 collision info */
}

#endif
