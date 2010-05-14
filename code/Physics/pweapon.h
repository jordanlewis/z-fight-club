#ifndef PWEAPON_H
#define PWEAPON_H

#include "physics.h"
#include "constants.h"
#include "Agents/agent.h"
#include "collision.h"

#include <iostream>

void useWeapons(Agent *agent);
void smackAll(Agent *agent, int force);
void smack(Agent *agent, int force);
void raygun(Agent *agent, int force);

#endif
