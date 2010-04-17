#ifndef POBJECT_H
#define POBJECT_H

#include "physics.h"
#include <iostream>

using namespace std;

class Physics;

class SphereInfo{
 public:
    dSpaceID space;
    float radius;
    SphereInfo(float radius, dSpaceID space = 0) {
	this->space = space; this->radius = radius;
    }
};

class BoxInfo{
 public:
    dSpaceID space;
    float lx, ly, lz;
    BoxInfo(float lx, float ly, float lz, dSpaceID space = 0){
	this->space = space; this->lx = lx; this->ly = ly; this->lz = lz;
    }
};

class CylinderInfo{
    
};

class CCylinderInfo{
    
};

class PlaneInfo{
    
};

class RayInfo{

};

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
    PGeom(Physics *physics, PlaneInfo info); //NYI
    PGeom(Physics *physics, RayInfo info); //NYI
};


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
	      CylinderInfo info);
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      CCylinderInfo info);
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      PlaneInfo info);
    PMoveable(Physics *physics, Kinematic *kinematic, float mass,
	      RayInfo info);
    void odeToKinematic(); //writes (syncs) the body coords into the kinematic
    void kinematicToOde();
};

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
	   float mass, CylinderInfo info);
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering, 
	   float mass, CCylinderInfo info);
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	   float mass, PlaneInfo info);
    PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	   float mass, RayInfo info);
    void steeringToOde();
};
#endif
