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
    vector<WorldObject *>::iterator iter;
    PAgent *p;
    Agent *a;
    int nSteps, i;
    float desiredTimeSteps;


    dt += timeStepRemainder * PH_TIMESTEP;

    desiredTimeSteps = dt / PH_TIMESTEP;
    nSteps = floorf(desiredTimeSteps);
    timeStepRemainder = desiredTimeSteps - nSteps;

    for (i = 0; i < nSteps; i++)
    {
        for (iter = world.wobjects.begin(); iter != world.wobjects.end();iter++)
        {
            if ((*iter)->gobject && (*iter)->gobject->geominfo) {
                ParticleSystemInfo *particles = dynamic_cast<ParticleSystemInfo *>((*iter)->gobject->geominfo);
                if (particles) {
                    /* we have a particle system */
                    particles->update(dt);
                }
            }
            if (!(*iter)->agent)
                continue;
            a = (*iter)->agent;
            p = dynamic_cast<PAgent *>(a->worldObject->pobject);
            p->kinematicToOde();
            p->steeringToOde();
            useWeapons(a);
        }

        dSpaceCollide(odeSpace, this, &nearCallback);
        dWorldStep(odeWorld, PH_TIMESTEP);
        dJointGroupEmpty(odeContacts);

        for (iter = world.wobjects.begin(); iter != world.wobjects.end();iter++)
        {
            if (!(*iter)->agent)
                continue;
            a = (*iter)->agent;
            p = dynamic_cast<PAgent *>((*iter)->pobject);
            const Kinematic &k = p->odeToKinematic();
            a->setKinematic(k);
            p->resetOdeAngularVelocity(1);
        }
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

