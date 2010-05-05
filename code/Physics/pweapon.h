#ifndef PWEAPON_H
#define PWEAPON_H

#include "physics.h"
#include "constants.h"
#include "Agents/agent.h"
#include "collision.h"

#include <iostream>

void useWeapons(Agent *agent);
void smackAll(unsigned int srcAgentID, int force);
void smack(unsigned int tarAgentID, int force);
void raygun(unsigned int srcAgentID, int force);

#endif
