#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include <ext/hash_map>
#include <vector>
#include <cmath>
#include <iostream>
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "pobject.h"

class PGeom;
class PMoveable;
class PAgent;

#define GRAVITY -9.8
#define LINDAMP .001
#define ANGDAMP .01


class Physics
{
    __gnu_cxx::hash_map<int, PAgent *> pagents;
    dWorldID odeWorld;
    dSpaceID odeSpace;
    dJointGroupID odeContacts;

    void updateAgentKinematic(Agent::Agent *agent, float dt);

    static Physics _instance;
    Physics();
    ~Physics();
    Physics(const Physics&);
    Physics &operator=(const Physics&);
  public:
    std::vector<PGeom *> pgeoms;
    const dWorldID & getOdeWorld() { return odeWorld; };
    const dSpaceID & getOdeSpace() { return odeSpace; };
    const dJointGroupID & getOdeContacts() { return odeContacts; };
    void makeTrackGeoms();
    void initPhysics();
    void initAgent(Agent &agent);
    void simulate(float dt); /* step the world forward by dt. */

    static Physics &getInstance();
};
#endif
