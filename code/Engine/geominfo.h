#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>
#include "Parser/objLoader.h"
#include "Utilities/vector.h"
#include "Utilities/load-png.h"

/* Stores info common to all geoms.  Cannot be constructed directly -- must be
   constructed by a child class */
class GeomInfo
{
 public:
    virtual dGeomID createGeom(dSpaceID space) = 0;
    virtual void createMass(dMass *, float) {return;};
    /*! \brief draw() simply render the appropriate shape for this geom type.
     * the caller will deal with position and orientation setup. */
    virtual void draw() {return;};
};

//Packages info for a Sphere of radius r
class SphereInfo : public GeomInfo
{
 public:
    float radius;
    SphereInfo(float radius);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw();
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw();
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d);
    dGeomID createGeom(dSpaceID space);
};

//Packages info for a ray of length len
class RayInfo : public GeomInfo
{
 public: 
    float len;
    RayInfo(float len);
    dGeomID createGeom(dSpaceID space);
};

class TriMeshInfo : public GeomInfo
{
 public:
    dTriMeshDataID meshID;
    int nVerts;
    const Vec3f_t * verts;
    int nTris;
    const int * tris;
    Vec3f_t * normals;
    TriMeshInfo(dTriMeshDataID meshID, int nVerts, const Vec3f_t * verts,
                                       int nTris,  const int * tris,
                                       Vec3f_t * normals);
    dGeomID createGeom(dSpaceID space);
    void draw();
};

class ObjMeshInfo : public GeomInfo
{
    public:
	objLoader obj; /* contains the data as well as loading it */
	ObjMeshInfo(char *filename);
	~ObjMeshInfo();
	void load(char *filename);
	dGeomID createGeom(dSpaceID space);
	void createMass(dMass *, float);
	void draw();
};
#endif
