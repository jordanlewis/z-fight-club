#ifndef GOBJECT_H
#define GOBJECT_H
#include "Engine/geominfo.h"
#include "Utilities/vec3f.h"
#include "Utilities/quat.h"
#include "Utilities/matrix.h"
#include "Agents/agent.h"

class GObject
{
    GeomInfo *geominfo;
  public:
    GObject(GeomInfo *geominfo);
    void draw(Vec3f pos, Quatf_t quat);
    void draw(Vec3f pos, Quatf_t quat, Agent *agent);
};
#endif
