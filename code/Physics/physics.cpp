#include <math.h>
#include <ode/ode.h>
#include <ext/hash_map>

#include "physics.h"
#include "Utilities/vector.h"
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Utilities/vec3f.h"

#define MAX_CONTACTS 8

using namespace std;

Physics Physics::_instance;

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    // don't collide if the two bodies are connected by a normal joint
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;
    // don't collide if both objects are just part of the static environment
    if (!b1 && !b2)
        return;

    Physics *p = (Physics *) data;
    dWorldID odeWorld = p->getOdeWorld();
    dJointGroupID odeContacts = p->getOdeContacts();


    PGeom *g1 = (PGeom *)dGeomGetData(o1);
    PGeom *g2 = (PGeom *)dGeomGetData(o2);

    float bounce = (g1->bounce + g2->bounce)*.5;
    float mu1, mu2;

    dVector3 rel_vel = {0, 0, 0};
    float norm = 0;

    if (g1->mu1 == dInfinity || g2->mu1 == dInfinity) mu1 = dInfinity;
    else mu1 = (g1->mu1 + g2->mu1)*.5;
	
    if (g1->mu2 == dInfinity || g2->mu2 == dInfinity) mu2 = dInfinity;
    else mu2 = (g1->mu2 + g2->mu2)*.5;
    
    int mode = 0;
    
    if (bounce > 1) bounce = 1;
    if (bounce < 0) bounce = 0;
    if (mu1 < 0) mu1 = 0;
    if (mu2 < 0) mu2 = 0;

    if (bounce > 0) mode = mode | dContactBounce;
    if (mu2 > 0) mode = mode | dContactMu2;

    dContact contact[MAX_CONTACTS];

    int numCollisions = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                                 sizeof(dContact));
    if (numCollisions > 0)
    {
        cout << numCollisions << " collisions detected" << endl;
        for (int i = 0; i < numCollisions; i++)
        {
            contact[i].surface.mode = mode;
            contact[i].surface.mu = mu1;
            contact[i].surface.mu2 = mu2;
            contact[i].surface.bounce = bounce;
            contact[i].surface.bounce_vel = 0.1;

	    //This is all horrendously wrong, but it's a start.
	    //I mean... really horrendously wrong.  But will get fixed soon(TM)
	    if (b1 != 0) 
		{
		dBodyGetPointVel(b1, contact[i].geom.pos[0],
				 contact[i].geom.pos[1],
				 contact[i].geom.pos[2], contact[i].fdir1);
		}
	    else if (b2 != 0) 
		{
		dBodyGetPointVel(b2, contact[i].geom.pos[0],
				 contact[i].geom.pos[1],
				 contact[i].geom.pos[2], rel_vel);
		}
	    
	    norm = 0;
	    for(int j=0; j < 2; j++){
		contact[i].fdir1[j] -= rel_vel[j];
		norm += contact[i].fdir1[j]*contact[i].fdir1[j];
	    }
	    norm = sqrt(norm);
	    contact[i].surface.mu *= norm;
	    contact[i].surface.mu2 *= norm;
            /*
	    cout << "Friction coeff: " << contact[i].surface.mu << endl;
	    cout << "Bounce: " << bounce << endl;
            */

            dJointID c = dJointCreateContact(odeWorld, odeContacts, contact+i);
            dJointAttach(c, b1, b2);
        }
    }
}

void Physics::makeTrackGeoms()
{
    const TrackData_t *track = World::getInstance().getTrack();
    float depth = .1;
    float height = 2;
    float len;

    Vec2f_t xzwall;
    Vec3f_t wall;
    int i, j;
    float theta;
    Edge_t *e, *next;
    PGeom *geom;
    dQuaternion quat;
    Vec3f position;

    for (i = 0; i < track->nSects; i++)
    {
        for (j = 0; j < track->sects[i].nEdges; j++)
        {
            e = track->sects[i].edges + j;
            if (j == track->sects[i].nEdges - 1)
                next = e - 3;
            else
                next = e + 1;
            if (e->kind == WALL_EDGE)
            {
                SubV3f(track->verts[e->start], track->verts[next->start], wall);
                xzwall[0] = wall[0]; xzwall[1] = wall[2];
                len = LengthV2f(xzwall);
                BoxInfo box(len, height + wall[1], depth, 0, 200, 0, this->getOdeSpace());
                theta = atan2(wall[2] , wall[0]);
                geom = new PGeom(&box);
                pgeoms.push_back(geom);
                dQFromAxisAndAngle(quat, 0, 1, 0, -theta);
                geom->setQuat(quat);
                LerpV3f(track->verts[e->start], .5, track->verts[next->start],
                        wall);
                position = Vec3f(wall[0], wall[1], wall[2]);
                geom->setPos(position);
            }
        }
    }
}

void Physics::simulate(float dt)
{
    World &world = World::getInstance();
    vector<Agent *>::iterator iter;
    PAgent *p;
    Agent *a;
    for (iter = world.agents.begin(); iter != world.agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        p->kinematicToOde();
        p->steeringToOde();
    }

    dSpaceCollide(odeSpace, this, &nearCallback);
    dWorldStep(odeWorld, dt);
    dJointGroupEmpty(odeContacts);


    for (iter = world.agents.begin(); iter != world.agents.end(); iter++)
    {
        a = (*iter);
        p = pagents[a->id];
        const Kinematic &k = pagents[a->id]->odeToKinematic();
        a->setKinematic(k);
        p->resetOdeAngularVelocity();
    }

}

void Physics::initAgent(Agent &agent)
{
    Kinematic &k = agent.getKinematic();
    SteerInfo &s = agent.getSteering();
    BoxInfo geom = BoxInfo(agent.width, agent.height, agent.depth,
			   1, 0, 0, this->getOdeSpace());
    PAgent *pobj = new PAgent(&k, &s, 100, &geom);

    pagents[agent.id] = pobj;
}

void Physics::initPhysics()
{
    dInitODE();
    odeWorld = dWorldCreate();
    odeSpace = dHashSpaceCreate(0);
    odeContacts = dJointGroupCreate(0);

    dWorldSetAutoDisableFlag(odeWorld, 1);
    dWorldSetGravity(odeWorld, 0, GRAVITY, 0);
    dWorldSetLinearDamping(odeWorld, LINDAMP);
    dWorldSetAngularDamping(odeWorld, ANGDAMP);
}

Physics::Physics()
{
}

Physics::~Physics()
{
    dJointGroupDestroy(odeContacts);
    dSpaceDestroy(odeSpace);
    dWorldDestroy(odeWorld);
    dCloseODE();
}

Physics &Physics::getInstance()
{
    return _instance;
}

