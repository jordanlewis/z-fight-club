#include "agent.h"
#include "Utilities/vec3f.h"

unsigned int Agent::maxId = 0;    /* !<highest id number we've reached */
float        Agent::mass = AG_DEFAULT_MASS;   
float        Agent::power = AG_DEFAULT_POWER;  
float        Agent::maxRotate = AG_DEFAULT_MAX_ROTATE; 
float        Agent::height = AG_DEFAULT_HEIGHT;
float        Agent::width = AG_DEFAULT_WIDTH;
float        Agent::depth = AG_DEFAULT_DEPTH;

Agent::Agent() 
{
}

/* \brief initialize a SteerInfo class
 */
SteerInfo::SteerInfo() : acceleration(0), rotation(0), weapon(NONE), fire(0)
{
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent(Vec3f position) : steerInfo(), kinematic(position)
{
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent(Vec3f position, float orientation)
            : steerInfo(), kinematic(position, Vec3f(0,0,0), orientation)
{
}

/* \brief Calculate this agent's current maximum acceleration as a function
   \brief of power, mass, and current velocity.  Can return NaN.
*/
float Agent::getMaxAccel()
{
    static float pquotient = power/mass;
    return pquotient/kinematic.vel.length();
}

/* \brief get current kinematic for agent
 */
Kinematic &Agent::getKinematic ()
{
    return kinematic;
}

/* \brief get current kinematic for const agent
 */
const Kinematic &Agent::getKinematic () const
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
