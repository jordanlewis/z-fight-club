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
    start();
    static float timeStepRemainder;
    World &world = World::getInstance();
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
            if (!world.wobjects[j]->pobject)
                continue;
            world.wobjects[j]->pobject->prePhysics();
            if ((a = world.wobjects[j]->agent))
                {
                    useWeapons(a);
                    a->steerInfo.fire = 0;
                }
        }

        dSpaceCollide(odeSpace, NULL, &nearCallback);
        dWorldStep(odeWorld, PH_TIMESTEP);
        dJointGroupEmpty(odeContacts);

        for (j = 0; j < world.wobjects.size(); j++)
        {
            if (!world.wobjects[j]->pobject)
                continue;
            world.wobjects[j]->pobject->postPhysics();
            world.wobjects[j]->pobject->odeToKinematic();
            if ((a = world.wobjects[j]->agent))
            {
                a->setKinematic(static_cast<PAgent *>(world.wobjects[j]->pobject)->outputKinematic);
            }
        }
    }

    for (i = 0; i < world.particleSystems.size(); i++)
    {
        ParticleStreamObject * p = world.particleSystems[i];
        if (!p->alive)
        {
            delete p;
            world.particleSystems.erase(world.particleSystems.begin() + i--);
            continue;
        }

        p->gobject->geominfo->update(p, dt);
    }

    for (i = 0; i < world.wobjects.size(); i++)
    {
        if (!world.wobjects[i]->alive)
        {
            delete world.wobjects[i];
            world.wobjects.erase(world.wobjects.begin() + i--);
            continue;
        }

        PGeom *pg = world.wobjects[i]->pobject;
        if (!pg || pg->collidedWith.empty())
            continue;

        for (j = 0; j < pg->collidedWith.size(); j++)
        {
            /* Check to see both collidee and collider (pg) are still alive */
            if (!pg || !pg->worldObject || !pg->worldObject->alive)
                break;
            WorldObject *collidee = pg->collidedWith[j];
            if (!collidee || !collidee->alive || !collidee->pobject)
                continue;

            pg->collisionReact(collidee->pobject);
        }
        pg->collidedWith.clear();

    }
    finish();
}

Physics::Physics() :
    Component(), error(&Error::getInstance())
{
    frequency = 100;
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

