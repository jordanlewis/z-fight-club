#ifndef GOBJECT_H
#define GOBJECT_H

#include "allclasses.h"
#include "Utilities/quat.h"
#include "Utilities/vec3f.h"

class GObject
{
    GeomInfo *geominfo;
  public:
    GObject(GeomInfo *geominfo);
    void draw(Vec3f pos, Quatf_t quat);
    void draw(Vec3f pos, Quatf_t quat, Agent *agent);

    friend class Physics;
};
#endif
