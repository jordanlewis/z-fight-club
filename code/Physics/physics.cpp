#include <math.h>
#include <ode/ode.h>
#include <ext/hash_map>
#include <iostream>

#include "physics.h"
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Utilities/vec3f.h"

#define MAX_CONTACTS 8

using namespace std;

Physics Physics::_instance;

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

    int numCollisions = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                                 sizeof(dContact));
    if (numCollisions > 0)
    {
        cout << numCollisions << " collisions detected" << endl;
        for (int i = 0; i < numCollisions; i++)
        {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.mu2 = 0;
            contact[i].surface.bounce = 0.9;
            contact[i].surface.bounce_vel = 0.1;

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
    vector<Agent *>::iterator iter;
    PAgent *p;
    Agent *a;
    for (iter = world->agents.begin(); iter != world->agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        p->kinematicToOde();
        p->steeringToOde();
    }

    dSpaceCollide(odeSpace, this, &nearCallback);
    dWorldStep(odeWorld, dt);
    dJointGroupEmpty(odeContacts);


    for (iter = world->agents.begin(); iter != world->agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        const Kinematic &k = pagents[a->id]->odeToKinematic();
        a->setKinematic(k);
        p->resetOdeAngularVelocity();
    }

}

/*
void Physics::simulate(float dt)
{
    
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
    BoxInfo geom = BoxInfo(agent.width, agent.height, agent.depth,
			   .8, 0, 0, this->getOdeSpace());
    PAgent *pobj = new PAgent(&k, &s, 100, &geom);

    pagents[agent.id] = pobj;
}

void Physics::initPhysics()
{
    dInitODE();
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    odeContacts = dJointGroupCreate(0);

    dWorldSetAutoDisableFlag(odeWorld, 1);

}

Physics::Physics() : world(&World::getInstance())
{
}

Physics::~Physics()
{
    dJointGroupDestroy(odeContacts);
    dSpaceDestroy(odeSpace);
    dWorldDestroy(odeWorld);
    dCloseODE();
}

Physics &Physics::getInstance()
{
    return _instance;
}

