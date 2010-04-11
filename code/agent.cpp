#include "agent.h"

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent(Vec3f_t position)
{
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
    CopyV3f(position, pos.orig);
    ZeroV3f(pos.dir);
    this->orientation = orientation;
}

/* \brief set desired steering information for agent
 * \param steerInfo a SteerInfo object with the desired new parameters
 */
void Agent::SetSteering(SteerInfo &steerInfo)
{
    this->steerInfo = steerInfo;
}
