#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>

/* Stores info common to all geoms.  Cannot be constructed directly -- must be
   constructed by a child class */
class GeomInfo
{
 protected:
    GeomInfo(float bounce, float mu1, float mu2);
 public:
    float bounce; //How bouncy this surface is -- float in range [0, 1]
    float mu1, mu2; /* How much friction this surface has.  mu1 is in the 
		       direction of contact motion, mu2 is perpendicular.
		       takes values in the range [0, dInfinity] */
    virtual dGeomID createGeom() {return 0;};
    virtual void createMass(dMass *, float) {return;};
};

//Packages info for a Sphere of radius r
class SphereInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float radius;
    SphereInfo(float radius, 
	       float bounce, float mu1, float mu2, dSpaceID space);
    dGeomID createGeom();
    void createMass(dMass *, float);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz,
	    float bounce, float mu1, float mu2, dSpaceID space);
    dGeomID createGeom();
    void createMass(dMass *, float);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    dSpaceID space;
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d,
	      float bounce, float mu1, float mu2, dSpaceID space);
    dGeomID createGeom();
};

#endif
