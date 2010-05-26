#ifndef POBJECT_H
#define POBJECT_H

#include "allclasses.h"
#include "Agents/kinematic.h"
#include "Utilities/quat.h"
#include "constants.h"
#include <ode/ode.h>
#include <iostream>

using namespace std;

//How our objects will interact with the collision simulation
typedef enum {
    REAL = 0, //Acts and collides as you would expect an object to act
    PHANTOM // Phantom objects do not affect the physics simulation
} CollType_t;

/* Stores geometry info for use in ODE collision calculations
 * See geominfo.h for info on member variables.
 */
class PGeom
{
 protected:
    dGeomID geom;

    //Defining this function gives us a vtable.  The function does nothing.
    virtual void forceVtableCreation();

 public:
    dSpaceID space;
    float bounce;
    float mu1, mu2;
    CollType_t collType;
    WorldObject *worldObject;

    bool isPlaceable();
    Vec3f getPos();
    void setPos(Vec3f position);
    void getQuat(Quatf_t quat);
    void setQuat(Quatf_t quat);
    void ntohQuat(RPQuat *payload);
    void htonQuat(RPQuat *payload);

    const dGeomID &getGeom();
    PGeom(GeomInfo *info, dSpaceID space=NULL);
    ~PGeom();
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
    void resetToStopped(Vec3f pos, Quatf_t quat); /* Set pmoveable's ODE data
                                                     and stop its motion */

    const Kinematic &odeToKinematic(); /* writes (syncs) the body coords into
                                        * the kinematic */
    const dBodyID &getBody();
    void kinematicToOde(); //writes (syncs) the kinematic coords into the body
};

// Same as PMoveable, but adds steering info and related interfaces
class PAgent: public PMoveable
{
 protected:
    const SteerInfo *steering;
 public:
    PAgent(const Kinematic *kinematic, const SteerInfo *steering,
            float mass, GeomInfo *info, dSpaceID space=NULL);
    void kinematicToOde();
    void steeringToOde(); //Write steering info into the ODE structs
    void resetOdeAngularVelocity(int nsteps);
};
#endif
