#ifndef COLLISION_H
#define COLLISION_H

#include <list>
#include <cassert>

#include "physics.h"
#include "Engine/geominfo.h"
#include "Utilities/vector.h"
#include "Engine/world.h"

struct CollContact{
 public:
    //float depth; //contact distance from origin of cast -- only used in casts
    //WorldObject *obj; //The object that collided
    Vec3f_t position; //The point of contact
};

//  Space reserved for future additions to the CollQuery struct
struct CollQuery{
 public:
    list<CollContact> contacts;  //depth-sorted list of all contacts 
};

void nearCallback(void *data, dGeomID o1, dGeomID o2);

void RayCast(Rayf_t ray);

float ray_sphere(Rayf_t ray, Vec3f_t center, float radius); /* When does ray intersect sphere? */

#endif
