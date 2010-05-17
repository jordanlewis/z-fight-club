#ifndef GEOMINFO_H
#define GEOMINFO_H

#include <ode/ode.h>
#include "Utilities/vector.h"
#include <string>
extern "C" {
    #include "Parser/obj-reader.h"
    #include "Utilities/load-png.h"
    #include "Graphics/shader.h"
}

typedef enum {
    COLOR_TEX = 0, 
    BUMP_TEX,
    SPEC_TEX,
    NUM_TEXS
} Texids_t;

struct RPAttachPGeom;
struct RPGeomInfo;

//Used in network transfer.
typedef enum {
    SPHERE,
    BOX,
    PLANE,
    RAY,
    TRIMESH,
    OBJMESH
} GeomInfoType_t;

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
    virtual void ntoh(RPGeomInfo *payload) {return;};
    virtual void hton(RPGeomInfo *payload) {return;};
};

//Packages info for a Sphere of radius r
class SphereInfo : public GeomInfo
{
 public:
    float radius;
    SphereInfo(); //needed for networking
    SphereInfo(float radius);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw();
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz;
    BoxInfo(); //needed for networking
    BoxInfo(float lx, float ly, float lz);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw();
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
};


//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo : public GeomInfo
{
 public:
    float a, b, c, d;
    PlaneInfo(); //needed for networking
    PlaneInfo(float a, float b, float c, float d);
    dGeomID createGeom(dSpaceID space);
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
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
	OBJmodel	*model;			/* !< pointer to obj model */
	std::string	path;			/* !< path to model folder */
        ShaderProgram_t *shader;                /* !< shader to render with */
	GLuint		texIDs[NUM_TEXS];	/* !< texture identifiers */
	Image2D_t	*texs[NUM_TEXS];	/* image maps */
	ObjMeshInfo(std::string);
	~ObjMeshInfo();
	void load(char *filename);
	dGeomID createGeom(dSpaceID space);
	void createMass(dMass *, float);
	void draw();
};
#endif
