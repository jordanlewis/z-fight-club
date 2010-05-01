#ifndef GOBJECT_H
#define GOBJECT_H
#include "Engine/geominfo.h"
#include "Utilities/vec3f.h"
#include "Utilities/quat.h"
#include "Utilities/matrix.h"

class GObject
{
    GeomInfo *geominfo;
  public:
    GObject(GeomInfo *geominfo);
    void draw(Vec3f pos, Quatf_t quat);
    void draw(); /* !< dont' transform before drawing shape - for non-placeable
                  * geoms */
};
#endif
