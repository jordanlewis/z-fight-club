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
            case SMACK: smackAll(agent, PH_SMACKFORCE); break;
            case RAYGUN:  raygun(agent, PH_SMACKFORCE); break;
            default: break;
        }
    }
    return;
}

//Push all other agents in random directions
void smackAll(Agent * agent, int force)
{
    vector<WorldObject *>::iterator iter;
    World &world = World::getInstance();
    for (iter = world.wobjects.begin(); iter != world.wobjects.end(); iter++)
    {
        if (!(*iter)->agent)
            continue;

        if ((*iter)->agent != agent)
        {
            smack((*iter)->agent, force);
        }
    }
}

//Push a target agent in a random direction
void smack(Agent *agent, int force){
    PAgent *target = dynamic_cast<PAgent *>(agent->worldObject->pobject);
    int rng = rand();
    int rng2 = rand();
    int rng3 = rand();
    Vec3f f = Vec3f(sin(rng), sin(abs(rng2)), sin(rng3));
    f *= force;
    dBodyAddForce(target->getBody(), f[0], f[1], f[2]);
    dBodyAddTorque(target->getBody(), f[0], f[1], f[2]);
}

void raygun(Agent *agent, int force)
{
    vector<WorldObject *>::iterator iter;
    std::list<CollContact>::iterator citer;
    World &world = World::getInstance();
    for (iter = world.wobjects.begin(); iter != world.wobjects.end(); iter++)
    {
        if (!(*iter)->agent)
            continue;

        if ((*iter)->agent == agent){
            Agent *a = agent;
            Vec3f origin;
            Vec3f dir;
            float len = 10000;
            dir[0] = a->getKinematic().orientation_v[0];
            dir[1] = a->getKinematic().orientation_v[1];
            dir[2] = a->getKinematic().orientation_v[2];
            dir.normalize();
            origin[0] = a->getKinematic().pos[0];
            origin[1] = a->getKinematic().pos[1];
            origin[2] = a->getKinematic().pos[2];
            CollQuery query;
            rayCast(&origin, &dir, len, &query);
            for (citer = query.contacts.begin(); citer != query.contacts.end(); citer++)
            {
                //Force Push!
                if ((*citer).obj != NULL){
                    //cout << "Hit object: " << (*citer).obj << endl;
                    if ((*citer).obj->agent != NULL){
                        if ((*citer).obj->agent != agent){
                            smack((*citer).obj->agent, force);
                        }
                    }
                }
            }
        }
    }
}
