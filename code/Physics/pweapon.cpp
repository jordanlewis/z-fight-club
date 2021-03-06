#include "pweapon.h"
#include "pobject.h"
#include "collision.h"
#include "Agents/agent.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Graphics/gobject.h"
#include "Sound/sound.h"

/* Call once per physics update.  Checks to see if an agents is using weapons.
 * If so, perform the physically appropriate action.
 */
/* We need to cache these to avoid a file read each time we shoot an weapon
 * right now they just get deleted when the game quits, that's not so bad since
 * we do need them the whole time.
 */
static ObjMeshInfo *rocket = NULL;
static ObjMeshInfo *mine = NULL;

void useWeapons(Agent *agent)
{
    SteerInfo info = agent->getSteering();
    Sound *sound = &Sound::getInstance();
    /*cout << "Attempting to use weapons in physics.  info.fire is: "
      << info.fire << endl;*/ 
    if (info.fire == 1) {
        if (agent->ammo[info.weapon] > 0)
        {
            agent->ammo[info.weapon]--;
            Vec3f pos = agent->worldObject->getPos();
            switch(info.weapon) {
                case RAYGUN:
                    raygun(agent, PH_SMACKFORCE);
                    sound->addSoundAt("lazer.wav", GetTime(), AL_FALSE, 1.0,
                                      pos);

                    break;
                case ROCKET:
                    launchBox(agent);
                    sound->addSoundAt("rocketwhoosh.wav", GetTime(), AL_FALSE,
                                      1.0, pos);
                    break;
                case MINE:
                    launchMine(agent);
                    break;
                default:
                    break;
            }
        }
        else
        {
            /* No ammo! Make 'chk' noise or something. */
            sound->addSoundAt("empty.wav", GetTime(), AL_FALSE, 1.0,
                              agent->worldObject->getPos());
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
    PAgent *target = static_cast<PAgent *>(agent->worldObject->pobject);
    Vec3f f = randomVec3f(Vec3f(force, 1, force));
    Vec3f p = randomVec3f(Vec3f(1,1,1));
    dBodyAddForceAtRelPos(target->getBody(), f[0], f[1], f[2], p[0],0,p[2]);
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

    Vec3f area = agent->getKinematic().orientation_v * 50;
    Vec3f velocity = Vec3f(100.0, 0.0, 0.0);
    Vec3f velocity_pm = Vec3f(0, .3, .3);
    float ttl = 3.0;
    float ttl_pm = 1.0;
    float birthRate = 100.0;

    ParticleSystemInfo *particleSystem = new ParticleSystemInfo("particles/fire.png", area, velocity, velocity_pm, ttl, ttl_pm, birthRate);
    particleSystem->linearArea = true;
    GParticleObject *particle_gobj = new GParticleObject(particleSystem);
    ParticleStreamObject *particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL, 2);
    particle_wobj->setPos(agent->getKinematic().pos);

    World::getInstance().addObject(particle_wobj);

}

void launchBox(Agent *agent)
{
    if (!rocket)
        rocket = new ObjMeshInfo("Weapons/Rocket/");
    BoxInfo *box = new BoxInfo(.2,.2,.2);
    Kinematic &ak = agent->getKinematic();
    Kinematic *k = new Kinematic(ak.pos, ak.orientation_v * 25 );
    SteerInfo *s = new SteerInfo;
    s->acceleration = 50;
    k->pos += ak.orientation_v;
    k->orientation = ak.orientation;
    PProjectile *pobj = new PProjectile(k, s, 100, box);
    pobj->bounce = 1;
    GObject *gobj = new GObject(rocket);
    WorldObject *wobj = new WorldObject(pobj, gobj, NULL, NULL, 10);
    World::getInstance().addObject(wobj);

    /* create a particle generator for the agent */
    Vec3f position = Vec3f(0.0, .5, 0.0);
    Vec3f area = Vec3f(.01, .01, .01);
    Vec3f velocity = Vec3f(-1.0, 0.0, 0.0);
    Vec3f velocity_pm = Vec3f(0, .3, .3);
    float ttl = 2.0;
    float ttl_pm = 1.0;
    float birthRate = 25.0;

    ParticleSystemInfo *particleSystem = new ParticleSystemInfo("particles/beam.png", area, velocity, velocity_pm, ttl, ttl_pm, birthRate);
    GParticleObject *particle_gobj = new GParticleObject(particleSystem);
    ParticleStreamObject *particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL);
    wobj->addChild(particle_wobj);
    particle_wobj->setPos(position);


    World::getInstance().addObject(particle_wobj);
}

void launchMine(Agent *agent)
{
    if (!mine)
        mine = new ObjMeshInfo("Weapons/Mine/");
    BoxInfo *box = new BoxInfo(.2,.2,.2);
    Kinematic &ak = agent->getKinematic();
    Kinematic *k = new Kinematic(ak.pos - (ak.orientation_v), Vec3f(0,0,0));
    SteerInfo *s = new SteerInfo;

    k->orientation = ak.orientation;
    PProjectile *pobj = new PProjectile(k, s, 100, box);
    GObject *gobj = new GObject(mine);
    WorldObject *wobj = new WorldObject(pobj, gobj, NULL, NULL, 100);
    World::getInstance().addObject(wobj);

}
