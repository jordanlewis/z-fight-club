#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>

#include "vector.h"
#include "world.h"
#include "agent.h"

class Physics
{
    World *world; /* our world */
    void updateAgentKinematic(Agent::Agent *agent, float dt);
  public:
    Physics();             /* constructor; set world later with initPhysics*/
    Physics(World *world); /* constructor; initialize with a world */
    void initPhysics(World *world);
    void simulate(float dt); /* step the world forward by dt. */
};

/*A container grouping all of the various representations of an object in our
  game world */
class ObjectContainer
{
 private:
    Kinematic *kinematic; //A kinematic bound to this object
    dBodyID body; //an ODE body object that we will use to simulate physics
    dMass mass;   //an ODE mass object giving us this object's mass info
    dGeomID geom; //an ODE geom object used if our body is to be collidable
    void write_kinematic(); //writes (syncs) the body coords into the kinematic

 public:
    
    ObjectContainer();
    /* Instantiates this ObjectContainer with a kinematic already bound */
    ObjectContainer(Kinematic *kinematic);
	/*binds a kinematic to this object. The kinematic data and body data
	  should be kept in snyc thereafter*/
    void bind_kinematic(Kinematic *kinematic);

};

#endif
