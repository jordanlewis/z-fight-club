#ifndef POBJECT_H
#define POBJECT_H

#include "physics.h"
#include "geominfo.h"
#include <iostream>

using namespace std;

class Physics;

//Stores geometry info for use in ODE collision calculations
class PGeom
{
 protected:
    dGeomID geom;
 public:
    const float bounce;
    const float mu1, mu2;
    void setPos(Vec3f position);
    void setQuat(const dQuaternion rotation);
    dGeomID getGeom();
    PGeom(GeomInfo *info);
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
              GeomInfo *info);
    const Kinematic &odeToKinematic(); //writes (syncs) the body coords into the kinematic
    void kinematicToOde(); //writes (syncs) the kinematic coords into the body
};

// Same as PMoveable, but adds steering info and related interfaces
class PAgent: public PMoveable
{
 protected:
    const SteerInfo *steering;
 public:
    PAgent(const Kinematic *kinematic, const SteerInfo *steering,
           float mass, GeomInfo *info);
    void steeringToOde(); //Write steering info into the ODE structs
};
#endif
