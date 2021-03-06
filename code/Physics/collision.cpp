#include "collision.h"
#include "Physics/pobject.h"
#include "Sound/sound.h"
#include "Sound/sobject.h"
#include "Engine/world.h"
#include "Engine/geominfo.h"
#include "Agents/agent.h"

CollContact::CollContact () : obj(NULL), distance(0),
                              position(Vec3f(0,0,0)), normal(Vec3f(0,0,0))
{}

/* Collides real objects o1 and o2.  Ignores collisions between real o1 and
 * phantom o2.  Stores collision data in the data pointer for phantom 01 and
 * real o2 */
void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    Physics &p = Physics::getInstance();
    dWorldID odeWorld = p.getOdeWorld();
    dJointGroupID odeContacts = p.getOdeContacts();

    PGeom *g1 = (PGeom *)dGeomGetData(o1);
    PGeom *g2 = (PGeom *)dGeomGetData(o2);

    if (g1 == NULL || g2 == NULL){
        cout << "Null geom data pointer!" << endl;
    }
    //Don't collide with phantoms in position 2.
    if (g2->collType == PHANTOM)
        {
            return;
        }
    //get collision data, but create no joints
    if (g1->collType == PHANTOM){
        dContactGeom contact;
        //Do nothing if no contact occurs
        if (!dCollide(o1, o2, 1, &contact, sizeof(dContactGeom))){
            return;
        };
        assert(data != NULL);
        CollContact response;
        response.position[0] = contact.pos[0];
        response.position[1] = contact.pos[1];
        response.position[2] = contact.pos[2];
        response.normal[0] = contact.normal[0];
        response.normal[1] = contact.normal[1];
        response.normal[2] = contact.normal[2];
        response.distance = contact.depth;
        response.obj = g2->worldObject;
        assert(g2->worldObject != NULL);
        ((CollQuery *)data)->contacts.push_back(response);
        return;
    }
    // don't collide if the two bodies are connected by a normal joint
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;
    // don't collide if both objects are just part of the static environment
    if (!b1 && !b2)
        return;

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
        /* Register collisions with one object */
        g1->collidedWith.push_back(g2->worldObject);

        for (int i = 0; i < numCollisions; i++)
        {
            contact[i].surface.mode = mode;
            contact[i].surface.mu = mu1;
            contact[i].surface.mu2 = mu2;
            contact[i].surface.bounce = bounce;
            contact[i].surface.bounce_vel = 0.1;

            dJointID c = dJointCreateContact(odeWorld,
                                                odeContacts,
                                                contact+i);
            dJointAttach(c, b1, b2);
        }
    }
}

/* \Brief  Given a ray, fills the collQuery structure with collision info.
 */
void rayCast(const Vec3f *origin, const Vec3f *dir, float len, CollQuery *collQuery)
{
    Physics &physics = Physics::getInstance();
    RayInfo info = RayInfo(len);
    PGeom rayGeom(&info, physics.getOdeSpace());
    //dGeomID rayGeom = dCreateRay(physics.getOdeSpace(), ray.len);
    rayGeom.collType = PHANTOM;
    dGeomRaySetLength(rayGeom.getGeom(), info.len);
    dGeomRaySet(rayGeom.getGeom(), (*origin)[0], (*origin)[1], (*origin)[2],
                (*dir)[0], (*dir)[1], (*dir)[2]);
    dSpaceCollide2(rayGeom.getGeom(),
                   (dGeomID)physics.getOdeSpace(), collQuery, &nearCallback);
}
