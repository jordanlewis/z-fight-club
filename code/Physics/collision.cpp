#include "collision.h"

void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    //cout << "nearCallback called!" << endl;
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

    if (g1 == NULL || g2 == NULL){ 
	cout << "Null geom data pointer!" << endl;
    }

    /*cout << "colltypes: (" << g1->collType << ", " << g2->collType << ")" 
	 << endl;
    */
    //Don't collide with phantoms.
    if (g2->collType == PHANTOM)
	{
	    cout << "bad phantom detectd" << endl;
	    return;
	}
    //get collision data, but create no joints
    if (g1->collType == PHANTOM){
	cout << "Phantom detected" << endl;
	dContactGeom contact;
	dCollide(o1, o2, 1, &contact, sizeof(dContactGeom));
	assert(data != NULL);
	CollContact response;
	response.position[0] = contact.pos[0];
	response.position[1] = contact.pos[1];
	response.position[2] = contact.pos[2];
	response.obj = g2->worldObject;
	((CollQuery *)data)->contacts.push_back(response);
	return;
    }
    else {
	//Calculate bounce value
	float bounce = (g1->bounce + g2->bounce)*.5;
	float mu1, mu2;
	int mode = 0;
	//dVector3 rel_vel = {0, 0, 0};
	//float norm = 0;
	
	//Calculate friction parameters
	if (g1->mu1 == dInfinity || g2->mu1 == dInfinity) mu1 = dInfinity;
	else mu1 = (g1->mu1 + g2->mu1)*.5;
	
	if (g1->mu2 == dInfinity || g2->mu2 == dInfinity) mu2 = dInfinity;
	else mu2 = (g1->mu2 + g2->mu2)*.5;
	//Bounds checking on parameters.  Maybe make into an assert?
	if (bounce > 1) bounce = 1;
	if (bounce < 0) bounce = 0;
	if (mu1 < 0) mu1 = 0;
	if (mu2 < 0) mu2 = 0;
	//Set contact mode
	if (bounce > 0) mode = mode | dContactBounce;
	if (mu2 > 0) mode = mode | dContactMu2;
	
	dContact contact[PH_MAX_CONTACTS];
	
	int numCollisions = dCollide(o1, o2, PH_MAX_CONTACTS, &contact[0].geom,
				     sizeof(dContact));
	if (numCollisions > 0)
	    {
		//cout << numCollisions << " collisions detected" << endl;
		for (int i = 0; i < numCollisions; i++)
		    {
			contact[i].surface.mode = mode;
			contact[i].surface.mu = mu1;
			contact[i].surface.mu2 = mu2;
			contact[i].surface.bounce = bounce;
			contact[i].surface.bounce_vel = 0.1;
			/*
			//This is all horrendously wrong, but it's a start.
			//I mean... really horrendously wrong.
			//We'll fix this if we decide to use friction.
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
			*/
            /*
	      cout << "Friction coeff: " << contact[i].surface.mu << endl;
	      cout << "Bounce: " << bounce << endl;
            */
			
			dJointID c = dJointCreateContact(odeWorld,
							 odeContacts,
							 contact+i);
			dJointAttach(c, b1, b2);
		    }
	    }
    }
}

/* \Brief  Given a ray, fills the collQuery structure with collision info.
 */ 
void rayCast(Rayf_t ray, CollQuery &collQuery) {
    Physics &physics = Physics::getInstance();
    RayInfo info = RayInfo(ray.len);
    PGeom rayGeom(&info, physics.getOdeSpace());
    //dGeomID rayGeom = dCreateRay(physics.getOdeSpace(), ray.len);
    rayGeom.collType = PHANTOM;
    dGeomRaySetLength(rayGeom.getGeom(), info.len);
    dGeomRaySet(rayGeom.getGeom(), ray.orig[0], ray.orig[1], ray.orig[2], 
		ray.dir[0], ray.dir[1], ray.dir[2]);
    dSpaceCollide2(rayGeom.getGeom(), 
		   (dGeomID)physics.getOdeSpace(), &collQuery, &nearCallback);
}