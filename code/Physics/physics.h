#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include <ext/hash_map>
#include <cmath>
#include <iostream>
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "pobject.h"

class PGeom;
class PMoveable;
class PAgent;

class Physics
{
    World *world; /* our world */

    __gnu_cxx::hash_map<int, PAgent *> pagents;
    dWorldID odeWorld;
    dSpaceID odeSpace;
    dJointGroupID odeContacts;

    void updateAgentKinematic(Agent::Agent *agent, float dt);

    static Physics _instance;
    Physics();
    ~Physics();
    Physics(const Physics&);
    Physics &operator=(const Physics&);
  public:
    const dWorldID & getOdeWorld() { return odeWorld; };
    const dSpaceID & getOdeSpace() { return odeSpace; };
    const dJointGroupID & getOdeContacts() { return odeContacts; };
    void initPhysics();
    void initAgent(Agent &agent);
    void simulate(float dt); /* step the world forward by dt. */

    static Physics &getInstance();
};

/* Links ODE to our game world. */
/*
class PObject
{
    Physics *physics; // The physics object this PObject belongs to
    Kinematic *kinematic; //A kinematic bound to this object
    SteerInfo *steering; // Steering info bound to this object
    dBodyID body; //an ODE body object that we will use to simulate physics
    dMass mass;   //an ODE mass object giving us this object's mass info
    dGeomID geom; //an ODE geom object used if our body is to be collidable
  public:
    PObject(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
            float mass, float xDim, float yDim, float zDim);
    void odeToKinematic(); //writes (syncs) the body coords into the kinematic
    void kinematicToOde(); //writes (syncs) the kinematic into ODE
    void steeringToOde();  //translates steering info into ODE forces
};
*/
#endif
