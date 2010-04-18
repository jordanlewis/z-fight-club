#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>

class GeomInfo
{
  public:
    dSpaceID space;
    virtual dGeomID createGeom() = 0;
    virtual void createMass(dMass *, float) = 0;
};

class SphereInfo : public GeomInfo
{
 public:
    float radius;
    SphereInfo(float radius, dSpaceID space) {
        this->space = space; this->radius = radius;
    }
    dGeomID createGeom();
    void createMass(dMass *, float);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz, dSpaceID space){
        this->space = space; this->lx = lx; this->ly = ly; this->lz = lz;
    }
    dGeomID createGeom();
    void createMass(dMass *, float);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d, dSpaceID space){
        this->space = space;
        this->a = a; this->b = b; this->c = c; this->d = d;
    }
    dGeomID createGeom();
};

#endif
