#include <math.h>
#include <ode/ode.h>
#include <ext/hash_map>
#include <iostream>

#include "physics.h"
#include "vector.h"
#include "world.h"
#include "agent.h"

using namespace std;

void Physics::updateAgentKinematic(Agent::Agent *agent, float dt)
{
    Kinematic &oldk = agent->getKinematic();
    SteerInfo &s = agent->getSteering();

    Kinematic newk;

    /* Position' = position + velocity * time */
    ScaledAddV3f(oldk.pos, dt, oldk.vel, newk.pos);

    if (newk.pos[0] > world->xMax) newk.pos[0] = world->xMax;
    if (newk.pos[0] < 0) newk.pos[0] = 0;
    if (newk.pos[2] > world->zMax) newk.pos[0] = world->zMax;
    if (newk.pos[2] < 0) newk.pos[2] = 0;

    /* Orientation' = orientation + rotation * time */
    newk.orientation = oldk.orientation + s.rotation * dt;
    newk.orientation = fmodf(newk.orientation, 2 * M_PI);
    /* Update velocity vector so it lies along orientation */
    float speed = LengthV3f(oldk.vel);
    newk.vel[0] = sin(newk.orientation) * speed;
    newk.vel[1] = oldk.vel[1];
    newk.vel[2] = cos(newk.orientation) * speed;

    /* Velocity += acceleration * time */
    ScaledAddV3f(newk.vel, s.acceleration * dt, newk.vel, newk.vel);

}

void Physics::simulate(float dt)
{
    std::cout << "Simulating!" << endl;
    for (unsigned int i = 0; i < world->agents.size(); i++)
    {
        updateAgentKinematic(&world->agents[i], dt);
    }
}

void Physics::initPhysics()
{
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    for (unsigned int i = 0; i < world->agents.size(); i++)
    {
        Agent &agent = world->agents[i];
        Kinematic &k = agent.getKinematic();
        PObject *pobj = new PObject(this, &k, 100, agent.width, agent.height,
                                    agent.depth);

        pobj->kinematicToOde();

        pobjects[agent.id] = pobj;
    }
}

Physics::Physics(World *world)
{
    this->world = world;
}

PObject::PObject(Physics *physics, Kinematic *kinematic, float mass,
                 float xDim, float yDim, float zDim)
{
    this->physics = physics;
    this->kinematic = kinematic;
    // allocate a dynamics body and collisions geometry with given dimensions
    this->body = dBodyCreate(physics->getOdeWorld());
    this->geom = dCreateBox(physics->getOdeSpace(), xDim, yDim, zDim);
    // give mass to body
    dMassSetBox(&this->mass, 1.0f, xDim, yDim, zDim);
    dMassAdjust(&this->mass, mass); // random mass, should change
    // connect body to geometry
    dGeomSetBody(geom, body);
}

//Copys the kinematic info into ODE's representation
void PObject::kinematicToOde()
{
    Kinematic *k = kinematic;
    dQuaternion q;

    dBodySetPosition(body, k->pos[0], k->pos[1], k->pos[2]);
    // get orientation as angle around y axis; give that quat to the body
    dQFromAxisAndAngle(q, 0, 1, 0, kinematic->orientation);
    dBodySetQuaternion(body, q);
}

//Copys the ode info into the associated kinematic struct
void PObject::odeToKinematic(){

    dQuaternion q_result, q_base;
    float norm;
    const dReal *b_info;
    //this const dReal* is actually a quaternion
    const dReal* q_current = dBodyGetQuaternion(body);
    
    //Fill in kinematic angle
    dQFromAxisAndAngle(q_base, 0, 1, 0, 0);
    //Want: q_result = q_current*q_base*q_current^{-1}, so...
    //Step1:  q_result = q_current*q_base
    dQMultiply0(q_result, q_current, q_base);
    //Step2:  q_result = q_result*q_current^{-1}
    dQMultiply2(q_result, q_result, q_current);
    
    /*Project to X-Z plane (Ignore the Y component), renormalize, and 
      calculate rotation around the Y axis*/
    norm = sqrt(q_result[1]*q_result[1] + q_result[3]*q_result[3]);
    if (norm == 0) {
	cerr << "Error:  Agent facing directly upwards.  Setting kinematic" 
	     << "theta to 0." << endl;
	kinematic->orientation = 0;
    }
    else {
	//Normalize vectors  
	q_result[1] = q_result[1]/norm;
	q_result[3] = q_result[3]/norm;
	//Calculate theta
	kinematic->orientation = atan2(q_result[1], q_result[3]);
    }

    //Fill in kinematic position and velocty
    b_info = dBodyGetPosition(body);
    kinematic->pos[0] = b_info[0];
    kinematic->pos[1] = b_info[1];
    kinematic->pos[2] = b_info[2];
    b_info = dBodyGetLinearVel(body);
    kinematic->vel[0] = b_info[0];
    kinematic->vel[1] = b_info[1];
    kinematic->vel[2] = b_info[2];
}

