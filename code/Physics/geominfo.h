#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>

class GeomInfo
{
  public:
    virtual dGeomID createGeom() = 0;
    virtual void createMass(dMass *, float) = 0;
};

class SphereInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float radius;
    SphereInfo(float radius, dSpaceID space);
    dGeomID createGeom();
    void createMass(dMass *, float);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz, dSpaceID space);
    dGeomID createGeom();
    void createMass(dMass *, float);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d, dSpaceID space);
    dGeomID createGeom();
};

#endif
