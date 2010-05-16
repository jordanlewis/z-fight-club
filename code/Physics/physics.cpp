#include <math.h>
#include <ode/ode.h>

#include "physics.h"
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Utilities/vec3f.h"

using namespace std;

Physics Physics::_instance;

void Physics::simulate(float dt)
{
    Error& error = Error::getInstance();
    error.pin(P_PHYSICS);
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
            p = dynamic_cast<PAgent *>(a->worldObject->pobject);
            const Kinematic &k = p->odeToKinematic();
            a->setKinematic(k);
            p->resetOdeAngularVelocity(nSteps);
        }
    }


    error.pout(P_PHYSICS);
}

Physics::Physics()
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

