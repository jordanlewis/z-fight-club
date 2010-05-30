#ifndef PWEAPON_H
#define PWEAPON_H

#include "allclasses.h"
#include "constants.h"
#include <iostream>

void useWeapons(Agent *agent);
void smackAll(Agent *agent, int force);
void smack(Agent *agent, int force);
void raygun(Agent *agent, int force);
void launchBox(Agent *agent);
void launchMine(Agent *agent);

#endif
