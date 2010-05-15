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
#include "collision.h"

class PGeom;
class PMoveable;
class PAgent;

class Physics
{
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

    static Physics &getInstance();
};
#endif
