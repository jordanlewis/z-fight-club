#include "agent.h"

/* \brief initialize an agent class
 * \param position the agent's initial position
 */
Agent::Agent::Agent(Vec3f_t position) 
{
    engineState = NEUTRAL;
    turn_angle = 0.0f;
    CopyV3f(position, pos.orig);
    ZeroV3f(pos.dir);
    orientation[0] = orientation[1] = 0.0f;
    orientation[2] = 1.0f; /* facing into the z direction */
}

/* \brief initialize an agent class
 * \param position the agent's initial position
 * \param orientation the agent's initial orientation
 */
Agent::Agent::Agent(Vec3f_t position, Vec3f_t orientation)
{
    engineState = NEUTRAL;
    turn_angle = 0.0f;
    CopyV3f(position, pos.orig);
    ZeroV3f(pos.dir);
    CopyV3f(orientation, this->orientation);
}

/* \brief set engine state
 * \param engineState
 */
void Agent::Agent::EngineState(EngineState_t newState)
{
    engineState = newState;
}

/* \brief turn 
 * \param turnAngle
 */
void Agent::Agent::Turn(float newTurn_angle)
{
    turn_angle = newTurn_angle;
}
