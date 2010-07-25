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

/*! Abstract baseclass for representations of physical, possible drawable geometric objects.
 *
 * Subclasses of this class are meant to encapsulate geometrical information
 * about shapes, for example spheres, cylinders, or planes, that help with
 * construction meshes for the shapes, drawing them, and calculating their
 * volumes.
 *
 * Subclasses must know how to create an ODE dMass for themselves, an ODE
 * dGeomID for themselves, how to draw themselves (implemented in
 * drawgeoms.cpp), and how to serialize and deserialize themselves for network
 * send and receive.
 */
class GeomInfo
{
 public:
    /*! given an ODE space, create a dGeom that represents the shape of the
     *  subclass, and return its dGeomID. */
    virtual dGeomID createGeom(dSpaceID space) = 0;
    /*! given a pointer to an ODE dMass object and a mass value, populate the
     *  dMass object with the given mass value. */
    virtual void createMass(dMass *, float) {return;};
    /*! Render the appropriate shape for this geom type.
     * the caller will deal with position and orientation setup. */
    virtual void draw(Layer_t layer) {return;};
    /*! Deserialize from payload. */
    virtual void ntoh(RPGeomInfo *payload) {return;};
    /*! Serialize into payload. */
    virtual void hton(RPGeomInfo *payload) {return;};
};

/*! A GeomInfo representing a sphere with a radius.*/
class SphereInfo : public GeomInfo
{
 public:
    float radius; /*! The sphere's radius */
    SphereInfo();
    SphereInfo(float radius);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw(Layer_t);
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
};

/*! A GeomInfo representing a rectangular prism. */
class BoxInfo : public GeomInfo
{
 public:
    float lx, ly, lz; /*! The box's three side lengths.*/
    BoxInfo();
    BoxInfo(float lx, float ly, float lz);
    dGeomID createGeom(dSpaceID space);
    void createMass(dMass *, float);
    void draw(Layer_t);
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
};


/*! A GeomInfo representing a flat plane.
 *  planes don't have masses, and can't get drawn. */

class PlaneInfo : public GeomInfo
{
 public:
    /*! <a,b,c> defines the normal vector of the plane.
     *  d defines the plane's distance from the origin along the normal vector.
     */
    float a, b, c, d;
    PlaneInfo();
    PlaneInfo(float a, float b, float c, float d);
    dGeomID createGeom(dSpaceID space);
    void ntoh(RPGeomInfo *payload);
    void hton(RPGeomInfo *payload);
};

/*! A GeomInfo representing a ray. 
 *
 * Rays don't have mass, and can't get drawn. */
class RayInfo : public GeomInfo
{
 public:
    float len; /*!< The ray's length. */
    RayInfo(float len);
    dGeomID createGeom(dSpaceID space);
};

/*! A GeomInfo representing an arbitrary triangle mesh, in ODE's format.
 *
 * These can be used to create ODE TriMesh physics objects. */
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

/*! A GeomInfo representing an arbirary triangle mesh with fancier graphics constructs.
 * These can have textures and whatnot, unlike TriMeshInfos. */
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

/*! A GeomInfo representing a textured skybox. */
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

/*! Represents a particle of a particle system. */
class Particle
{
    public:
        Vec3f   pos; /*!< The particle's position. */
        Vec3f   vel; /*!< The particle's velocity. */
        float   ttl; /*!< How long the particle has to live, in seconds. */
        Particle(Vec3f, Vec3f, float);
        ~Particle();
};

bool isDead(Particle *); /*!< Returns true if the particle should die. */

/*! A GeomInfo representing a particle system.
 *
 * A particle system is made up of a bunch of Particle objects, and some meta
 * data about when the particles should be spawned, with what attributes, and
 * when to remove those particles.
 */
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
        bool linearArea; /*!< spawn particles in a line? off by default */
        std::list<Particle *>     particles;  /* !< the live particles in the system */
        ParticleSystemInfo(std::string, Vec3f, Vec3f, Vec3f, float, float, float);
        ParticleSystemInfo(std::string, Vec3f, Vec3f, Vec3f, float, float, float, int);
        ~ParticleSystemInfo();
        dGeomID createGeom(dSpaceID space);
        void createMass(dMass *, float);
        void draw(Layer_t);
        void update(ParticleStreamObject *, float);
};

/*!< Creates an explosion effect with a particle system. */
void makeExplosion(Vec3f, float);

#endif
