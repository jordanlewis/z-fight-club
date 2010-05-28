#ifndef POBJECT_H
#define POBJECT_H

#include "allclasses.h"
#include "Agents/kinematic.h"
#include "Utilities/quat.h"
#include "constants.h"
#include <ode/ode.h>
#include <iostream>
#include <list>

using namespace std;

//How our objects will interact with the collision simulation
typedef enum {
    REAL = 0, //Acts and collides as you would expect an object to act
    PHANTOM // Phantom objects do not affect the physics simulation
} CollType_t;

class PMoveable;
class PProjectile;
class PAgent;
class PBottomPlane;

/* Stores geometry info for use in ODE collision calculations
 * See geominfo.h for info on member variables.
 */
class PGeom
{
 protected:
    dGeomID geom;

 public:
    dSpaceID space;
    float bounce;
    float mu1, mu2;
    CollType_t collType;
    WorldObject *worldObject;

    vector<WorldObject *> collidedWith;

    virtual void collisionReact(PGeom *pg) {pg->doCollisionReact(this);}
    virtual void doCollisionReact(PGeom *pg);
    virtual void doCollisionReact(PMoveable *pm);
    virtual void doCollisionReact(PProjectile *pp);
    virtual void doCollisionReact(PAgent *pa);
    virtual void doCollisionReact(PBottomPlane *pb);

    bool isPlaceable();
    Vec3f getPos();
    void setPos(Vec3f position);
    void getQuat(Quatf_t quat);
    void setQuat(Quatf_t quat);
    void ntohQuat(RPQuat *payload);
    void htonQuat(RPQuat *payload);

    const dGeomID &getGeom();
    PGeom(GeomInfo *info, dSpaceID space=NULL);
    virtual ~PGeom();
};

class PBottomPlane : public PGeom
{
  public:
    PBottomPlane(GeomInfo *info, dSpaceID space=NULL);
    virtual void collisionReact(PGeom *pg) {pg->doCollisionReact(this);}
    virtual void doCollisionReact(PAgent *pa);
    virtual void doCollisionReact(PGeom *pg);
};

/*
 * Stores both dynamics and geometry info for use in ODE.  Provides methods to
 * synchronize kinematic interface with ODE
 */
class PMoveable: public PGeom
{
 protected:
    const Kinematic *kinematic;
    Kinematic outputKinematic;
    dBodyID body;
    dMass mass;

 public:
    PMoveable(const Kinematic *kinematic, float mass,
              GeomInfo *info, dSpaceID space=NULL);
    virtual ~PMoveable();

    virtual void collisionReact(PGeom *pg) {pg->doCollisionReact(this);}
    virtual void doCollisionReact(PGeom *pg);
    virtual void doCollisionReact(PMoveable *pm);
    virtual void doCollisionReact(PProjectile *pp);
    virtual void doCollisionReact(PAgent *pa);

    void resetToStopped(Vec3f pos, Quatf_t quat); /* Set pmoveable's ODE data
                                                     and stop its motion */
    const Kinematic &odeToKinematic(); /* writes (syncs) the body coords into
                                        * the kinematic */
    const dBodyID &getBody();
    void kinematicToOde(); //writes (syncs) the kinematic coords into the body
};

class PProjectile: public PMoveable
{
  public:
    PProjectile(const Kinematic *kinematic, float mass, GeomInfo *info,
                double liveTime=10, dSpaceID space=NULL);

    virtual void collisionReact(PGeom *pg) {pg->doCollisionReact(this);}
    virtual void doCollisionReact(PGeom *pg);
    virtual void doCollisionReact(PMoveable *pm);
    virtual void doCollisionReact(PProjectile *pp);
    virtual void doCollisionReact(PAgent *pa);


    bool destroy;
    double timeCreated;
    double liveTime;
};

// Same as PMoveable, but adds steering info and related interfaces
class PAgent: public PMoveable
{
 protected:
    const SteerInfo *steering;
 public:
    PAgent(const Kinematic *kinematic, const SteerInfo *steering,
            float mass, GeomInfo *info, dSpaceID space=NULL);

    virtual void collisionReact(PGeom *pg) {pg->doCollisionReact(this);}
    virtual void doCollisionReact(PGeom *pg);
    virtual void doCollisionReact(PMoveable *pm);
    virtual void doCollisionReact(PProjectile *pp);
    virtual void doCollisionReact(PAgent *pa);
    virtual void doCollisionReact(PBottomPlane *pb);

    void kinematicToOde();
    void steeringToOde(); //Write steering info into the ODE structs
    void resetOdeAngularVelocity(int nsteps);
};
#endif
