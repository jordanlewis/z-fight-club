#include "pobject.h"

#define DEBUG

//Create a spherical geometry
PGeom::PGeom(Physics *physics, SphereInfo info){
    this->physics = physics; 
    geom = dCreateSphere(info.space, info.radius);
}

//Create a boxy geometry
PGeom::PGeom(Physics *physics, BoxInfo info){
    this->physics = physics;
    geom = dCreateBox(info.space, info.lx, info.ly, info.lz);
}

//Create a planar geometry
PGeom::PGeom(Physics *physics, PlaneInfo info){
    this->physics = physics;
    geom = dCreatePlane(info.space, info.a, info.b, info.c, info.d);
}

//Create a spherical body and associated geometry
PMoveable::PMoveable(Physics *physics, Kinematic *kinematic, float mass,
		     SphereInfo info) : PGeom(physics, info){
    //Create a body, give it mass, and bind it to the geom
    body = dBodyCreate(physics->getOdeWorld());
    dMassSetSphereTotal(&this->mass, mass, info.radius);
    dBodySetMass(body, &this->mass);
    dGeomSetBody(geom, body);
    
    //Sync Kinematic info -- initial position and orientation
    this->kinematic = kinematic;
    kinematicToOde();
}

PMoveable::PMoveable(Physics *physics, Kinematic *kinematic, float mass,
		     BoxInfo info) : PGeom(physics, info){
    //Create a body, give it mass, and bind it to the geom
    body = dBodyCreate(physics->getOdeWorld());
    dMassSetBoxTotal(&this->mass, mass, info.lx, info.ly, info.lz);
    dBodySetMass(body, &this->mass);
    dGeomSetBody(geom, body);

    //Sync Kinematic info -- initial position and orientation
    this->kinematic = kinematic;
    kinematicToOde();
}

PAgent::PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	       float mass, SphereInfo info) : PMoveable(physics, kinematic,
							 mass, info) {
    this->steering = steering;
}

PAgent::PAgent(Physics *physics, Kinematic *kinematic, SteerInfo *steering,
	       float mass, BoxInfo info) : PMoveable(physics, kinematic,
							 mass, info) {
    this->steering = steering;
}

//Copys the kinematic info into ODE's representation
void PMoveable::kinematicToOde()
{
    Kinematic *k = kinematic;
    dQuaternion q;

    dBodySetPosition(body, k->pos[0], k->pos[1], k->pos[2]);
    // get orientation as angle around y axis; give that quat to the body
    dQFromAxisAndAngle(q, 0, 1, 0, kinematic->orientation);
    dBodySetQuaternion(body, q);
}

//Copys the ode info into the associated kinematic struct
void PMoveable::odeToKinematic(){

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
    
    /*Project to X-Z plane (Ignore the Y component), renormalize, and 
      calculate rotation around the Y axis*/
    norm = sqrt(q_result[1]*q_result[1] + q_result[3]*q_result[3]);
    if (norm == 0) {
	cerr << "Error:  Agent facing directly upwards.  Setting kinematic" 
	     << "theta to 0." << endl;
	kinematic->orientation = 0;
    }
    else {
	//Normalize vectors  
	q_result[1] = q_result[1]/norm;
	q_result[3] = q_result[3]/norm;
	//Calculate theta
	kinematic->orientation = atan2(q_result[1], q_result[3]);
	//cout << "Calculated orientation as " << kinematic->orientation << endl;
    }

    //Fill in kinematic position and velocty
    b_info = dBodyGetPosition(body);
    kinematic->pos[0] = b_info[0];
    kinematic->pos[1] = b_info[1];
    kinematic->pos[2] = b_info[2];
    b_info = dBodyGetLinearVel(body);
    kinematic->vel[0] = b_info[0];
    kinematic->vel[1] = b_info[1];
    kinematic->vel[2] = b_info[2];

#ifdef DEBUG
    int prec = cout.precision(2);
    ios::fmtflags flags = cout.setf(ios::fixed,ios::floatfield);
    cout << "Body " << body << ": pos" << kinematic->pos
         << " vel" << kinematic->vel << " dir "
         << kinematic->orientation << endl;
    cout.setf(flags,ios::floatfield);
    cout.precision(prec);
#endif
}


//Translates the object's steering info into ODE forces
void PAgent::steeringToOde()
{
    const dReal* angVel = dBodyGetAngularVel(body);
    dBodySetAngularVel(body, angVel[0], steering->rotation, angVel[2]);

    Vec3f f = Vec3f(sin(kinematic->orientation),0,cos(kinematic->orientation));
    f *= steering->acceleration * mass.mass;
    dBodyAddForce(body, f[0], f[1], f[2]);
}
