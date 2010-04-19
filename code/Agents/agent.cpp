#include "agent.h"
#include "Utilities/vec3f.h"

unsigned int Agent::maxId = 0;    /* !<highest id number we've reached */
float        Agent::maxAccel = 10;  /*XXX 10 is a random value; tweak this */
float        Agent::maxRotate = .3; /*XXX .3 is a random value; tweak this */
float        Agent::height = 5;
float        Agent::width = 10;
float        Agent::depth = 40;

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent(Vec3f position)
{
    id = maxId++; /* Get next ID, increment maxId */
    kinematic.pos = position;
    kinematic.orientation = 0; /* facing into the z direction */
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent(Vec3f position, float orientation)
{
    id = maxId++;
    kinematic.pos = position;
    kinematic.orientation = orientation;
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
