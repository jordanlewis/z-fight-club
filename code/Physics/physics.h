#ifndef PHYSICS_H
#define PHYSICS_H

#include <ode/ode.h>
#include <ext/hash_map>
#include <vector>
#include <cmath>
#include <iostream>
#include "allclasses.h"
#include "constants.h"
#include "Utilities/quat.h"

void QuatfToDQuat(Quatf_t quatf, dQuaternion dquat);
void DQuatToQuatf(dQuaternion dquat, Quatf_t quatf);

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
    Error *error;
  public:
    const dWorldID & getOdeWorld() { return odeWorld; };
    const dSpaceID & getOdeSpace() { return odeSpace; };
    const dJointGroupID & getOdeContacts() { return odeContacts; };
    void simulate(float dt); /* step the world forward by dt. */

    static Physics &getInstance();
};
#endif
