#include "pweapon.h"

/* Call once per physics update.  Checks to see if an agents is using weapons.
 * If so, perform the physically appropriate action.
 */ 
void useWeapons(Agent *agent){
    SteerInfo info = agent->getSteering();
    if (info.fire == 1) {
	//cout << "Fire, FIRE!" << endl; // Firing once too often. Fix later.
	switch(info.weapon){
	    case NONE: break;
	    case SMACK: smackAll(agent->id, PH_SMACKFORCE); break;
	    default: break;
	}
    }
    return;
}

//Push all other agents in random directions
void smackAll(unsigned int srcAgentID, int force){
    for (vector<Agent *>::iterator iter = World::getInstance().agents.begin();
	 iter != World::getInstance().agents.end();
	 iter++){
	if ((*iter)->id != srcAgentID){
	    smack((*iter)->id, force);
	}
    }
}

//Push a target agent in a random direction
void smack(unsigned int tarAgentID, int force){
    PAgent *target = Physics::getInstance().getAgentMap()[tarAgentID];
    int rng = rand();
    Vec3f f = Vec3f(sin(rng), 0, cos(rng));
    f *= force;
    dBodyAddForce(target->getBody(), f[0], f[1], f[2]);
}
