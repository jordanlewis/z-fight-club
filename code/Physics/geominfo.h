#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>

#define D_BOUNCE 0  //Default bounce value
#define D_MU1 0     //Default mu1
#define D_MU2 0     //Default mu2
#define D_COLL REAL //Default collision type 

//How our geom will interact with the collision simulation
typedef enum {
    REAL = 0, //Acts and collides as you would expect an object to act
    CAMERA /* Collision with camera obj only changes another object's alpha.
	    * In particular, no change in position occurs.  This is the
	    * behavior corresponding to the "cameraman" behind a player. */
} CollType_t;

/* Stores info common to all geoms.  Cannot be constructed directly -- must be
   constructed by a child class */
class GeomInfo
{
 protected:
    GeomInfo();
    GeomInfo(float bounce, float mu1, float mu2, CollType_t collType);
 public:
    float bounce; //How bouncy this surface is -- float in range [0, 1]
    float mu1, mu2; /* How much friction this surface has.  mu1 is in the 
		       direction of contact motion, mu2 is perpendicular.
		       takes values in the range [0, dInfinity] */
    CollType_t collType; //What collision type is this geom?
    virtual dGeomID createGeom(dSpaceID space) {return 0;};
    virtual void createMass(dMass *, float) {return;};
};

//Packages info for a Sphere of radius r
class SphereInfo : public GeomInfo
{
 public:
    float radius;
    SphereInfo(float radius);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d);
    dGeomID createGeom(dSpaceID space);
};

class TriMeshInfo : public GeomInfo
{
 public:
    dTriMeshDataID meshID;
    const void * verts;
    const void * tris;
    TriMeshInfo(dTriMeshDataID meshID, const void * verts, const void * tris
);
    dGeomID createGeom(dSpaceID space);
};

#endif
