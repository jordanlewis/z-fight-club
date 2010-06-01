#ifndef GEOMINFO_H
#define GEOMINFO_H

#include "allclasses.h"
#include <ode/ode.h>
#include "Utilities/vec3f.h"
#include "Graphics/graphics.h"
#include <string>
#include <list>
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

typedef enum {
    NORTH = 0,          /* !< +Z */
    EAST,               /* !< +X */
    SOUTH,              /* !< -Z */
    WEST,               /* !< -X */
    UP,                 /* !< +Y */
    DOWN,               /* !< -Y */
    NUM_DIR
} Direction_t;

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
    virtual void draw(Layer_t layer) {return;};
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
    void draw(Layer_t);
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
    void draw(Layer_t);
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
    void draw(Layer_t);
};

class ObjMeshInfo : public GeomInfo
{
  private:
    GLuint displayList;    /* !< the displayList used to render the object */
    uint32_t mode;         /* !< determines rendering options for this object */
  public:
	OBJmodel	*model;			/* !< pointer to obj model */
	std::string	path;			/* !< path to model folder */
	GLuint		colorTexId;	    /* !< texture identifiers */
	GLuint		glowTexId;	    /* !< texture identifiers */
	ObjMeshInfo(std::string);
	~ObjMeshInfo();
	void load(char *filename);
	dGeomID createGeom(dSpaceID space);
	void createMass(dMass *, float);
	void draw(Layer_t);
};

class SkyBoxInfo : public GeomInfo
{
    public:
        GLuint          texID[NUM_DIR];         /* !< ids for tex maps */
        SkyBoxInfo(std::string);
        ~SkyBoxInfo();
        dGeomID createGeom(dSpaceID space);
        void createMass(dMass *, float);
        void draw(Layer_t);
};

class Particle
{
    public:
        Vec3f   pos;
        Vec3f   vel;
        float   ttl;
        Particle(Vec3f, Vec3f, float);
        ~Particle();
};

bool isDead(Particle *);

class ParticleSystemInfo : public GeomInfo
{
    public:
        GLuint                  texid;        /* !< the texture to put on the sprite */
        Vec3f                   pos;          /* !< last position as set in update */
        Vec3f                   area;         /* !< upper corner of the box particles can spawn in */
        Vec3f                   velocity;     /* !< the starting velocity of the particles */
        Vec3f                   velocity_pm;  /* !< variance in velocity */
        float                   ttl;          /* !< lifetime of each particle */
        float                   ttl_pm;       /* !< variance in lifetime of each particle */
        float                   birthRate;    /* !< mean number of particle births per second */
        float                   lastUpdate;   /* !< time the Particle system was last updated */
        int                     maxParticles; /* !< set a maximum number of particles to spawn */
        std::list<Particle *>     particles;  /* !< the live particles in the system */
        ParticleSystemInfo(std::string, Vec3f, Vec3f, Vec3f, float, float, float);
        ParticleSystemInfo(std::string, Vec3f, Vec3f, Vec3f, float, float, float, int);
        ~ParticleSystemInfo();
        dGeomID createGeom(dSpaceID space);
        void createMass(dMass *, float);
        void draw(Layer_t);
        void update(ParticleStreamObject *, float);
};

void makeExplosion(Vec3f, float);

#endif
