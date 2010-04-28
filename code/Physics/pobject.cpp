#include "pobject.h"
#include <ode/ode.h>

#define DEBUG

#define D_BOUNCE 0  //Default bounce value
#define D_MU1 0     //Default mu1
#define D_MU2 0     //Default mu2
#define D_COLL REAL //Default collision type

PGeom::PGeom(GeomInfo *info, dSpaceID space)
    : geom(info->createGeom(space)), space(space), bounce(D_BOUNCE),
      mu1(D_MU1), mu2(D_MU2), collType(D_COLL)
{
    dGeomSetData(geom, this);
}

PMoveable::PMoveable(const Kinematic *kinematic, float mass,
                     GeomInfo *info, dSpaceID space)
                    : PGeom(info, space), kinematic(kinematic)
{
    //Create a body, give it mass, and bind it to the geom
    body = dBodyCreate(Physics::getInstance().getOdeWorld());
    info->createMass(&this->mass, mass);

    dBodySetMass(body, &this->mass);
    dGeomSetBody(geom, body);

    //Sync Kinematic info -- initial position and orientation
    kinematicToOde();
}

PAgent::PAgent(const Kinematic *kinematic, const SteerInfo *steering,
               float mass, GeomInfo *info, dSpaceID space)
              : PMoveable(kinematic, mass, info, space), steering(steering)
{
}


void PGeom::setPos(Vec3f position)
{
    dGeomSetPosition(geom, position[0], position[1], position[2]);
}

void PGeom::setQuat(const dQuaternion rotation)
{
    dGeomSetQuaternion(geom, rotation);
}

const dGeomID &PGeom::getGeom()
{
    return geom;
}

const dBodyID &PMoveable::getBody()
{
    return body;
}

/* \brief Copys the kinematic info into ODE's representation
 */
void PMoveable::kinematicToOde()
{
    const Kinematic *k = kinematic;
    dQuaternion q;

    dBodySetPosition(body, k->pos[0], k->pos[1], k->pos[2]);
    // get orientation as angle around y axis; give that quat to the body
    dQFromAxisAndAngle(q, 0, 1, 0, kinematic->orientation);
    dBodySetQuaternion(body, q);
}

/* /brief Copys the ode info into the associated kinematic struct
 */
const Kinematic &PMoveable::odeToKinematic(){
    Kinematic &k = outputKinematic;

    dQuaternion q_result, q_result1, q_base;
    float norm;
    const dReal *b_info;
    //this const dReal* is actually a quaternion
    const dReal* q_current = dBodyGetQuaternion(body);
    
    //Fill in kinematic angle
    q_base[0]=0; q_base[1] = 0; q_base[2] = 0; q_base[3]=1;
    //Want: q_result = q_current*q_base*q_current^{-1}, so...
    //Step1:  q_result = q_current*q_base
    dQMultiply0(q_result1, q_current, q_base);
    //Step2:  q_result = q_result*q_current^{-1}
    dQMultiply2(q_result, q_result1, q_current);

    //Write this result into kinematic's orientation_v
    k.orientation_v[0] = q_result[1];
    k.orientation_v[1] = q_result[2];
    k.orientation_v[2] = q_result[3];

    //Calculate and write the orientation projected onto the X-Z plane

    /*Project to X-Z plane (Ignore the Y component), renormalize, and 
      calculate rotation around the Y axis*/
    norm = sqrt(q_result[1]*q_result[1] + q_result[3]*q_result[3]);
    if (norm == 0) {
	cerr << "Error:  Agent facing directly upwards.  Setting kinematic" 
	     << "theta to 0." << endl;
	k.orientation = 0;
    }
    else {
	//Normalize vectors  
	q_result[1] = q_result[1]/norm;
	q_result[3] = q_result[3]/norm;
	//Calculate theta
	k.orientation = atan2(q_result[1], q_result[3]);
	//cout << "Calculated orientation as " << k.orientation << endl;
    }

    //Fill in kinematic position and velocty
    b_info = dBodyGetPosition(body);
    k.pos[0] = b_info[0];
    k.pos[1] = b_info[1];
    k.pos[2] = b_info[2];
    b_info = dBodyGetLinearVel(body);
    k.vel[0] = b_info[0];
    k.vel[1] = b_info[1];
    k.vel[2] = b_info[2];

#ifdef DEBUG
    int prec = cout.precision(2);
    ios::fmtflags flags = cout.setf(ios::fixed,ios::floatfield);
    //cout << "Body " << body << ": pos" << k.pos
    //     << " vel" << k.vel << " dir "
    //     << k.orientation << endl;
    cout.setf(flags,ios::floatfield);
    cout.precision(prec);
#endif
    return k;
}


/* \brief Translates the object's steering info into ODE forces and angular
 * \brief velocity.
 * \warning Since we add the angular velocity component directly to ODE's
 * \warning angular velocity for this object, we must remember to call
 * \warning agent.resetOdeAngularVelocity() after stepping ODE. And make sure
 * \warning that steering.rotation doesn't change until after that call.
 */
void PAgent::steeringToOde()
{
    const dReal* angVel = dBodyGetAngularVel(body);
    if (steering->acceleration || steering->rotation)
        dBodyEnable(body);

    dBodySetAngularVel(body, angVel[0], angVel[1] + steering->rotation,
      angVel[2]);

    Vec3f f = Vec3f(sin(kinematic->orientation),0,cos(kinematic->orientation));
    if (steering->acceleration < MAXACC && steering->acceleration > -MAXACC) {
        f *= steering->acceleration * mass.mass;
    }
    else if (steering->acceleration < -MAXACC)
        f *= -MAXACC * mass.mass;
    else if (steering->acceleration > MAXACC)
        f *= MAXACC * mass.mass;

    dBodyAddForce(body, f[0], f[1], f[2]);
}

/* \brief subtracts the artificially injected angular velocity from SteerInfo
 * \brief from ODE's conception of the body's angular velocity. this is to be
 * \brief called after stepping ODE for nSteps steps.
 */
void PAgent::resetOdeAngularVelocity(int nSteps)
{
    //return;
    const dReal* angVel = dBodyGetAngularVel(body);
    //cout << "Modding by rotation: " << steering->rotation << endl;
    dBodySetAngularVel(body, angVel[0], 
		       angVel[1] - steering->rotation*pow(1-ANGDAMP, nSteps),
		       angVel[2]);
    
}
