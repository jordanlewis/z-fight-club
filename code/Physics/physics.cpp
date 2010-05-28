#include <math.h>
#include <ode/ode.h>
#include "physics.h"
#include "pweapon.h"
#include "pobject.h"
#include "collision.h"
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Utilities/error.h"
#include "Agents/agent.h"
#include "Utilities/vec3f.h"
#include "Utilities/quat.h"
#include "Engine/geominfo.h"
#include "Graphics/gobject.h"

using namespace std;

Physics Physics::_instance;

void QuatfToDQuat(Quatf_t quatf, dQuaternion dquat)
{
    dquat[1] = quatf[0];
    dquat[2] = quatf[1];
    dquat[3] = quatf[2];
    dquat[0] = quatf[3];
}

void DQuatToQuatf(dQuaternion dquat, Quatf_t quatf)
{
    quatf[0] = dquat[1];
    quatf[1] = dquat[2];
    quatf[2] = dquat[3];
    quatf[3] = dquat[0];
}

void Physics::simulate(float dt)
{
    error->pin(P_PHYSICS);
    static float timeStepRemainder;
    World &world = World::getInstance();
    PAgent *p;
    Agent *a;
    unsigned int nSteps, i, j;
    float desiredTimeSteps;


    dt += timeStepRemainder * PH_TIMESTEP;

    desiredTimeSteps = dt / PH_TIMESTEP;
    nSteps = floorf(desiredTimeSteps);
    timeStepRemainder = desiredTimeSteps - nSteps;

    for (i = 0; i < nSteps; i++)
    {
        for (j = 0; j < world.wobjects.size(); j++)
        {
            if (! (a = world.wobjects[j]->agent))
                continue;
            p = static_cast<PAgent *>(a->worldObject->pobject);
            p->kinematicToOde();
            p->steeringToOde();
            useWeapons(a);
        }
        for (j = 0; j < world.particleSystems.size(); j++)
        {
            ParticleStreamObject * p = world.particleSystems[j];
            Vec3f pos;
            if (p->parent)
                pos = p->getPos() + p->parent->getPos();
            else
                pos = p->getPos();
            p->gobject->geominfo->update(pos, dt);
        }

        dSpaceCollide(odeSpace, NULL, &nearCallback);
        dWorldStep(odeWorld, PH_TIMESTEP);
        dJointGroupEmpty(odeContacts);

        for (j = 0; j < world.wobjects.size(); j++)
        {
            if (! (a = world.wobjects[j]->agent))
                continue;
            p = static_cast<PAgent *>(a->worldObject->pobject);
            const Kinematic &k = p->odeToKinematic();
            a->setKinematic(k);
            p->resetOdeAngularVelocity(1);
        }
    }
    for (i = 0; i < world.wobjects.size(); i++)
    {
        if (!world.wobjects[i]->pobject)
            continue;
        PGeom *pg = world.wobjects[i]->pobject;
        if (pg->collidedWith.empty())
            continue;

        for (vector<WorldObject *>::iterator iter = pg->collidedWith.begin();
             iter != pg->collidedWith.end(); iter++)
        {
            PGeom * collidee = (*iter)->pobject;
            /* Check to see both collidee and collider (pg) are still alive */
            if (!pg || !pg->worldObject)
                break;
            if (!collidee || !collidee->worldObject)
                continue;

            pg->collisionReact(collidee);
        }
        pg->collidedWith.clear();

    }
    error->pout(P_PHYSICS);
}

Physics::Physics() :
    error(&Error::getInstance())
{
    dInitODE();
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    odeContacts = dJointGroupCreate(0);

    dWorldSetAutoDisableFlag(odeWorld, 1);
    dWorldSetGravity(odeWorld, 0, PH_GRAVITY, 0);
    dWorldSetLinearDamping(odeWorld, PH_LINDAMP);
    dWorldSetAngularDamping(odeWorld, PH_ANGDAMP);
}

Physics::~Physics()
{
    if (odeContacts)
    {
        dJointGroupDestroy(odeContacts);
    }
    if (odeSpace)
    {
        dSpaceDestroy(odeSpace);
    }
    if (odeWorld)
    {
        dWorldDestroy(odeWorld);
    }
    dCloseODE();
}

Physics &Physics::getInstance()
{
    return _instance;
}

