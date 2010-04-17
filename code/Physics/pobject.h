#ifndef POBJECT_H
#define POBJECT_H

#include "physics.h"
#include <iostream>

using namespace std;

class Physics;

//Packages info for a sphere of radius r
class SphereInfo{
 public:
    dSpaceID space;
    float radius;
    SphereInfo(float radius, dSpaceID space = 0) {
	this->space = space; this->radius = radius;
    }
};

//Packages info for a box of dimensions lx*ly*lz
class BoxInfo{
 public:
    dSpaceID space;
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz, dSpaceID space = 0){
	this->space = space; this->lx = lx; this->ly = ly; this->lz = lz;
    }
};

//NYI
class CylinderInfo{
    
};

//NYI
class CCylinderInfo{
    
};

//Packages info for the plane defined by ax+by+cz = d;
class PlaneInfo{
 public:
    dSpaceID space;
    float a, b, c, d;
    PlaneInfo(float a, float b, float c, float d, dSpaceID space = 0){
	this->space = space;
	this->a = a; this->b = b; this->c = c; this->d = d;
    }
};

//NYI
class RayInfo{

};

//Stores geometry info for use in ODE collision calculations
class PGeom
{
 protected:
    Physics *physics; // The physics object this PGeom belongs to
    dGeomID geom;
 public:
    PGeom(Physics *physics, SphereInfo info);
    PGeom(Physics *physics, BoxInfo info);
    PGeom(Physics *physics, CylinderInfo info); //NYI
    PGeom(Physics *physics, CCylinderInfo info); //NYI
    PGeom(Physics *physics, PlaneInfo info);
    PGeom(Physics *physics, RayInfo info); //NYI
};

/*
 * Stores both dynamics and geometry info for use in ODE.  Provides methods to
 * synchronize kinematic interface with ODE
 */
class PMoveable: public PGeom 
{
 protected:
    Kinematic *kinematic;
    dBodyID body;
    dMass mass;
 public:
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      SphereInfo info);
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      BoxInfo info);
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      CylinderInfo info); //NYI
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      CCylinderInfo info); //NYI
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      RayInfo info); //NYI
    void odeToKinematic(); //writes (syncs) the body coords into the kinematic
    void kinematicToOde(); //writes (syncs) the kinematic coords into the body
};

// Same as PMoveable, but adds steering info and related interfaces
class PAgent: public PMoveable
{
 protected:
    SteerInfo *steering;
 public:
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering, 
	   float mass, SphereInfo info);
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	   float mass, BoxInfo info);
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	   float mass, CylinderInfo info); //NYI
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering, 
	   float mass, CCylinderInfo info); //NYI
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	   float mass, RayInfo info); //NYI
    void steeringToOde(); //Write steering info into the ODE structs
};
#endif
