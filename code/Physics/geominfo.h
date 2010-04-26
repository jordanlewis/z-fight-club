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
    virtual dGeomID createGeom(dSpaceID space) {return 0;};
    virtual void createMass(dMass *, float) {return;};
};

//Packages info for a Sphere of radius r
class SphereInfo : public GeomInfo
{
 public:
    float radius;
    SphereInfo(float radius, float bounce, float mu1, float mu2);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz, float bounce, float mu1, float mu2);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d,
	      float bounce, float mu1, float mu2);
    dGeomID createGeom(dSpaceID space);
};

class TriMeshInfo : public GeomInfo
{
 public:
    dTriMeshDataID meshID;
    const void * verts;
    const void * tris;
    TriMeshInfo(dTriMeshDataID meshID, const void * verts, const void * tris,
                float bounce, float mu1, float mu2);
    dGeomID createGeom(dSpaceID space);
};

#endif
