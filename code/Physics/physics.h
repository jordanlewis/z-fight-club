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
#include "pweapon.h"
#include "constants.h"

class PGeom;
class PMoveable;
class PAgent;

class Physics
{
    __gnu_cxx::hash_map<int, PAgent *> pagents;
    dWorldID odeWorld;
    dSpaceID odeSpace;
    dJointGroupID odeContacts;

    static Physics _instance;
    Physics();
    ~Physics();
    Physics(const Physics&);
    Physics &operator=(const Physics&);
    inline void simulateStep();
  public:
    const dWorldID & getOdeWorld() { return odeWorld; };
    const dSpaceID & getOdeSpace() { return odeSpace; };
    const dJointGroupID & getOdeContacts() { return odeContacts; };
    void simulate(float dt); /* step the world forward by dt. */
    __gnu_cxx::hash_map<int, PAgent *> &getAgentMap();

    static Physics &getInstance();
};
#endif
