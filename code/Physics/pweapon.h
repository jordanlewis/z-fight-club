#ifndef PWEAPON_H
#define PWEAPON_H

#include "physics.h"
#include "constants.h"
#include "Agents/agent.h"


void useWeapons(Agent *agent);
void smackAll(unsigned int srcAgentID, int force);
void smack(unsigned int tarAgentID, int force);

#endif
