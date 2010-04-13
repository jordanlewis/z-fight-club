#include "agent.h"
#include "vector.h"

unsigned int Agent::maxId = 0;    /* !<highest id number we've reached */
float        Agent::maxAccel = 10;  /*XXX 10 is a random value; tweak this */
float        Agent::maxRotate = .3; /*XXX .3 is a random value; tweak this */

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent(Vec3f_t position)
{
    id = maxId++; /* Get next ID, increment maxId */
    CopyV3f(position, pos.orig);
    ZeroV3f(pos.dir);
    orientation = 0; /* facing into the z direction */
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent(Vec3f_t position, float orientation)
{
    id = maxId++;
    CopyV3f(position, pos.orig);
    ZeroV3f(pos.dir);
    this->orientation = orientation;
}

void Agent::getKinematic (Kinematic *kinematic)
{
    CopyV3f(pos.orig, kinematic->pos);
    CopyV3f(pos.dir, kinematic->vel);
    kinematic->orientation = orientation;
}

/* \brief set desired steering information for agent
 * \param steerInfo a SteerInfo object with the desired new parameters
 */
void Agent::setSteering(SteerInfo &steerInfo)
{
    this->steerInfo = steerInfo;
}
