#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include <ext/hash_map>
#include "vector.h"
#include "world.h"
#include "agent.h"

class PObject;

class Physics
{
    World *world; /* our world */

    __gnu_cxx::hash_map<int, PObject *> pobjects;
    dWorldID odeWorld;
    dSpaceID odeSpace;
    void updateAgentKinematic(Agent::Agent *agent, float dt);
  public:
    Physics(World *world); /* constructor; initialize with a world */
    const dWorldID & getOdeWorld() { return odeWorld; };
    const dSpaceID & getOdeSpace() { return odeSpace; };
    void initPhysics();
    void simulate(float dt); /* step the world forward by dt. */
};

/* Links ODE to our game world. */
class PObject
{
    Physics *physics; // The physics object this PObject belongs to
    Kinematic *kinematic; //A kinematic bound to this object
    dBodyID body; //an ODE body object that we will use to simulate physics
    dMass mass;   //an ODE mass object giving us this object's mass info
    dGeomID geom; //an ODE geom object used if our body is to be collidable
  public:
    PObject(Physics *physics, Kinematic *kinematic, float mass,
            float xDim, float yDim, float zDim);
    void odeToKinematic(); //writes (syncs) the body coords into the kinematic
    void kinematicToOde(); //writes (syncs) the kinematic into ODE
};

#endif
