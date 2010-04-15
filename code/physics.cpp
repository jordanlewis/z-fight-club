#include <math.h>
#include <ode/ode.h>
#include "physics.h"
#include "vector.h"
#include "world.h"
#include "agent.h"

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

void PObject::kinematicToOde()
{
    Kinematic *k = kinematic;
    dQuaternion q;

    dBodySetPosition(body, k->pos[0], k->pos[1], k->pos[2]);
    // get orientation as angle around y axis; give that quat to the body
    dQFromAxisAndAngle(q, 0, 1, 0, kinematic->orientation);
    dBodySetQuaternion(body, q);
}
