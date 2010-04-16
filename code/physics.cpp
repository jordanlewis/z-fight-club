#include <math.h>
#include <ode/ode.h>
#include <ext/hash_map>
#include <iostream>

#include "physics.h"
#include "vector.h"
#include "world.h"
#include "agent.h"
#include "vec3f.h"

#define MAX_CONTACTS 8

using namespace std;

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    Physics *p = (Physics *) data;

    dWorldID odeWorld = p->getOdeWorld();
    dJointGroupID odeContacts = p->getOdeContacts();

    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    // don't collide if the two bodies are connected by a normal joint
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;

    dContact contact[MAX_CONTACTS];
    for (unsigned int i = 0; i < MAX_CONTACTS; i++)
    {
        contact[i].surface.mode = dContactBounce | dContactSoftCFM;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.mu2 = 0;
        contact[i].surface.bounce = 0.1;
        contact[i].surface.bounce_vel = 0.1;
        contact[i].surface.soft_cfm = 0.01;
    }

    int numCollisions = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                                 sizeof(dContact));
    if (numCollisions > 0)
    {
        dMatrix3 RI;
        dRSetIdentity (RI);
        for (int i = 0; i < numCollisions; i++)
        {
            dJointID c = dJointCreateContact (odeWorld, odeContacts, contact+i);
            dJointAttach(c, b1, b2);
        }
    }
}


void Physics::updateAgentKinematic(Agent::Agent *agent, float dt)
{
    Kinematic &oldk = agent->getKinematic();
    SteerInfo &s = agent->getSteering();

    Kinematic newk;

    /* Position' = position + velocity * time */
    newk.pos = oldk.pos + dt * oldk.vel;

    if (newk.pos[0] > world->xMax) newk.pos[0] = world->xMax;
    if (newk.pos[0] < 0) newk.pos[0] = 0;
    if (newk.pos[2] > world->zMax) newk.pos[0] = world->zMax;
    if (newk.pos[2] < 0) newk.pos[2] = 0;

    /* Orientation' = orientation + rotation * time */
    newk.orientation = oldk.orientation + s.rotation * dt;
    newk.orientation = fmodf(newk.orientation, 2 * M_PI);
    /* Update velocity vector so it lies along orientation */
    float speed = oldk.vel.length();
    newk.vel[0] = sin(newk.orientation) * speed;
    newk.vel[1] = oldk.vel[1];
    newk.vel[2] = cos(newk.orientation) * speed;

    /* Velocity += acceleration * time */
    newk.vel += s.acceleration * dt * newk.vel;

}

void Physics::simulate(float dt)
{
    for (vector<Agent>::iterator iter = world->agents.begin();
         iter != world->agents.end(); iter++)
    {
        PObject *p = pobjects[iter->id];
        p->kinematicToOde();
        p->steeringToOde();
    }

    dSpaceCollide(odeSpace, this, &nearCallback);
    dWorldStep(odeWorld, dt);
    dJointGroupEmpty(odeContacts);


    for (vector<Agent>::iterator iter = world->agents.begin();
         iter != world->agents.end(); iter++)
    {
        pobjects[iter->id]->odeToKinematic();
    }

}

/*
void Physics::simulate(float dt)
{
    cout << "Simulating!" << endl;
    for (unsigned int i = 0; i < world->agents.size(); i++)
    {
        updateAgentKinematic(&world->agents[i], dt);
    }
}
*/

void Physics::initAgent(Agent &agent)
{
    Kinematic &k = agent.getKinematic();
    SteerInfo &s = agent.getSteering();
    PObject *pobj = new PObject(this, &k, &s, 100, agent.width,
                                agent.height, agent.depth);

    pobj->kinematicToOde();

    pobjects[agent.id] = pobj;
}

void Physics::initPhysics()
{
    dInitODE();
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    odeContacts = dJointGroupCreate(0);

    dWorldSetAutoDisableFlag(odeWorld, 1);
    dWorldSetContactMaxCorrectingVel(odeWorld, 0.1);
    dWorldSetContactSurfaceLayer(odeWorld, 0.001);

}

Physics::Physics(World *world)
{
    this->world = world;
}

Physics::~Physics()
{
    dJointGroupDestroy(odeContacts);
    dSpaceDestroy(odeSpace);
    dWorldDestroy(odeWorld);
    dCloseODE();
}

PObject::PObject(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
                 float mass, float xDim, float yDim, float zDim)
{
    this->physics = physics;
    this->kinematic = kinematic;
    this->steering = steering;
    // allocate a dynamics body and collisions geometry with given dimensions
    this->body = dBodyCreate(physics->getOdeWorld());

    // set initial position and rotation
    dBodySetPosition(body, kinematic->pos[0], kinematic->pos[1],
                     kinematic->pos[2]);
    dQuaternion q;
    dQFromAxisAndAngle(q, 0, 1, 0, kinematic->orientation);
    dBodySetQuaternion(body, q);

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

//Translates the object's steering info into ODE forces
void PObject::steeringToOde()
{
    const dReal* angVel = dBodyGetAngularVel(body);
    dBodySetAngularVel(body, angVel[0], steering->rotation, angVel[2]);

    Vec3f f = Vec3f(sin(kinematic->orientation),0,cos(kinematic->orientation));
    f *= steering->acceleration * mass.mass;
    dBodyAddForce(body, f[0], f[1], f[2]);
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

    cout << kinematic->pos << endl;
}

