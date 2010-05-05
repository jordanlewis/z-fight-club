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
	    case RAYGUN:  raygun(agent->id, PH_SMACKFORCE); break;
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
    int rng2 = rand();
    int rng3 = rand();
    Vec3f f = Vec3f(sin(rng), sin(abs(rng2)), sin(rng3));
    f *= force;
    dBodyAddForce(target->getBody(), f[0], f[1], f[2]);
    dBodyAddTorque(target->getBody(), f[0], f[1], f[2]);
}

void raygun(unsigned int srcAgentID, int force){
    
    for (vector<Agent *>::iterator iter = World::getInstance().agents.begin();
	 iter != World::getInstance().agents.end();
	 iter++){
	if ((*iter)->id == srcAgentID){
	    Agent *a = (*iter);
	    Rayf_t ray;
	    ray.len = 10000;
	    ray.dir[0] = a->getKinematic().orientation_v[0];
	    ray.dir[1] = a->getKinematic().orientation_v[1];
	    ray.dir[2] = a->getKinematic().orientation_v[2];
	    NormalizeV3f(ray.dir);
	    ray.orig[0] = a->getKinematic().pos[0];
	    ray.orig[1] = a->getKinematic().pos[1];
	    ray.orig[2] = a->getKinematic().pos[2];
	    
	    CollQuery query;
	    rayCast(ray, query);
	    for (std::list<CollContact>::iterator iter=query.contacts.begin();
		 iter != query.contacts.end();
		 iter++){
		//Force Push!
		if ((*iter).obj != NULL){
		    //cout << "Hit object: " << (*iter).obj << endl;
		    if ((*iter).obj->agent != NULL){
			if ((*iter).obj->agent->id != srcAgentID){
			    smack((*iter).obj->agent->id, force);
			}
		    }
		}
	    }
	}
    }
}
