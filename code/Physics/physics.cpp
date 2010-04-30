#include <math.h>
#include <ode/ode.h>
#include <ext/hash_map>

#include "physics.h"
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Utilities/vec3f.h"

using namespace std;

Physics Physics::_instance;

__gnu_cxx::hash_map<int, PAgent *> &Physics::getAgentMap(){
    return pagents;
}

void Physics::simulate(float dt)
{
    static float dtRemainder;
    World &world = World::getInstance();
    vector<Agent *>::iterator iter;
    PAgent *p;
    Agent *a;
    int nSteps, i;
    float nTimeSteps;

    dt += dtRemainder * PH_TIMESTEP;

    for (iter = world.agents.begin(); iter != world.agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        p->kinematicToOde();
        p->steeringToOde();
	useWeapons(a);
    }
    nTimeSteps = dt / PH_TIMESTEP;
    nSteps = floorf(nTimeSteps);
    dtRemainder = nTimeSteps - nSteps;

    for (i = 0; i < nSteps; i++)
    {
        dSpaceCollide(odeSpace, this, &nearCallback);
        dWorldStep(odeWorld, PH_TIMESTEP);
        dJointGroupEmpty(odeContacts);
    }


    for (iter = world.agents.begin(); iter != world.agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        const Kinematic &k = pagents[a->id]->odeToKinematic();
        a->setKinematic(k);
        p->resetOdeAngularVelocity(nSteps);
    }

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

