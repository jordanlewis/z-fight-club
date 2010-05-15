#include "kinematic.h"
#include "Network/network.h"
#include "Network/racerpacket.h"

// ---------- Stream Output ----------
std::ostream &operator<<(std::ostream &os, const Kinematic &k)
{
    os << "Position: " << k.pos << std::endl;
    os << "Velocity: " << k.vel << std::endl;
    return os;
}
Kinematic::Kinematic(Vec3f pos, Vec3f vel, float orientation)
                    : pos(pos), vel(vel), orientation(orientation),
                      orientation_v(Vec3f(0,0,0))
{
}

float Kinematic::forwardSpeed() const
{
    return vel.dot(orientation_v);
}

void Kinematic::hton(RPKinematic *payload){
    payload->posx = htonf(pos.x);
    payload->posy = htonf(pos.y);    
    payload->posz = htonf(pos.z);

    payload->velx = htonf(vel.x);
    payload->vely = htonf(vel.y);
    payload->velz = htonf(vel.z);

    payload->orientx = htonf(orientation_v.x);
    payload->orienty = htonf(orientation_v.y);
    payload->orientz = htonf(orientation_v.z);

    payload->orientation = htonf(orientation);
}

void Kinematic::ntoh(RPKinematic *payload){
    pos.x = ntohf(payload->posx);
    pos.y = ntohf(payload->posy);
    pos.z = ntohf(payload->posz);

    vel.x = ntohf(payload->velx);
    vel.y = ntohf(payload->vely);
    vel.z = ntohf(payload->velz);

    orientation_v.x = ntohf(payload->orientx);
    orientation_v.y = ntohf(payload->orienty);
    orientation_v.z = ntohf(payload->orientz);
    
    orientation = ntohf(payload->orientation);
}
