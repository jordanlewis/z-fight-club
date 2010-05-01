#ifndef SOBJECT_H
#define SOBJECT_H
#include "Utilities/vec3f.h"
#include "Utilities/quat.h"
#include "Utilities/matrix.h"

class SObject
{
    // some secret sound information
  public:
    SObject();
    void do_your_thing(Vec3f pos, Quatf_t quat);
};
#endif
