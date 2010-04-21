#include "agent.h"
#include "Utilities/vec3f.h"

unsigned int Agent::maxId = 0;    /* !<highest id number we've reached */
float        Agent::maxAccel = 10;  /*XXX 10 is a random value; tweak this */
float        Agent::maxRotate = 2; /*XXX .3 is a random value; tweak this */
float        Agent::height = 2;
float        Agent::width = 2;
float        Agent::depth = 2;

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent(Vec3f position) : steerInfo(), id(maxId++), kinematic(position)
{
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent(Vec3f position, float orientation)
            : steerInfo(), id(maxId++), kinematic(position, Vec3f(0,0,0),
                                                  orientation)
{
}

/* \brief get current kinematic for agent
 */
Kinematic &Agent::getKinematic ()
{
    return kinematic;
}

/* \brief set kinematic for agent
 * \param kinematic a Kinematic object with the desired new parameters
 */
void Agent::setKinematic (const Kinematic &kinematic)
{
    this->kinematic = kinematic;
}

/* \brief get current steering information for agent
 */
SteerInfo &Agent::getSteering ()
{
    return steerInfo;
}

/* \brief set desired steering information for agent
 * \param steerInfo a SteerInfo object with the desired new parameters
 */
void Agent::setSteering(const SteerInfo &steerInfo)
{
    this->steerInfo = steerInfo;
}

std::ostream &operator<<(std::ostream &os, const Agent &agent)
{
    os << agent.kinematic << std::endl;
    return os;
}
