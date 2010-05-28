#ifndef COLLISION_H
#define COLLISION_H

#include "allclasses.h"
#include "physics.h"
#include "Utilities/vec3f.h"
#include <list>
#include <cassert>

struct CollContact{
 public:
    //float depth; //contact distance from origin of cast -- only used in casts
    WorldObject *obj; /*!< The object that collided */
    float distance; /*!< the length of the ray until the contact point */
    Vec3f position; /*!< The point of contact */
    Vec3f normal; /*!< normal vector to point of contact */
    CollContact ();
};

//  Space reserved for future additions to the CollQuery struct
struct CollQuery{
 public:
     std::list<CollContact> contacts;  //depth-sorted list of all contacts
};

void nearCallback(void *data, dGeomID o1, dGeomID o2);

void rayCast(const Vec3f *origin, const Vec3f *dir, float len, CollQuery *collQuery);



#endif
