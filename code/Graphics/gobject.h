#ifndef GOBJECT_H
#define GOBJECT_H

#include "allclasses.h"
#include "Utilities/quat.h"
#include "Utilities/vec3f.h"
#include "Graphics/graphics.h"

class GObject
{
    GeomInfo *geominfo;
  public:
    float agentBank;
    GObject(GeomInfo *geominfo);
    void draw(Vec3f, Quatf_t, Layer_t);
    void draw(Vec3f, Quatf_t, Agent*, Layer_t);

    friend class Physics;
};

class GParticleObject : public GObject
{
    ParticleSystemInfo *geominfo;
  public:
    GParticleObject(ParticleSystemInfo *psysteminfo);

  friend class Physics;
};

#endif
