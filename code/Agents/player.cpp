#include "player.h"
#include "agent.h"


void PlayerController::setTurnState(TurnState_t newState)
{
    turnState = newState;
}

void PlayerController::setEngineState(EngineState_t newState)
{
    engineState = newState;
}

void PlayerController::setWeaponState(WeaponState_t newstate)
{
    weaponState = newstate;
}

const TurnState_t &PlayerController::getTurnState()
{
    return turnState;
}

const EngineState_t &PlayerController::getEngineState()
{
    return engineState;
}

const WeaponState_t &PlayerController::getWeaponState()
{
    return weaponState;
}

const Agent &PlayerController::getAgent() const
{
    return *agent;
}

void PlayerController::updateAgent()
{
    if (!agent)
        return;
    SteerInfo steerInfo;
    steerInfo.weapon = agent->getSteering().weapon;
    switch (engineState) {
        case BRAKE:
        case NEUTRAL: steerInfo.acceleration = 0; break;
        case ACCELERATE: steerInfo.acceleration = agent->getMaxAccel(); break;
        case REVERSE: steerInfo.acceleration = -agent->getMaxAccel(); break;
        default: break;
    }
    switch (turnState) {
        case STRAIGHT: steerInfo.rotation = 0; break;
        case RIGHT:    steerInfo.rotation = -agent->maxRotate; break;
        case LEFT:     steerInfo.rotation = agent->maxRotate; break;
        default: break;
    }
    switch (weaponState) {
        case FIRE: steerInfo.fire = 1; weaponState = HOLD; break;
        case HOLD: steerInfo.fire = 0; break;
        case CHANGE: 
	    steerInfo.weapon = (Weapon_t)((int)steerInfo.weapon + 1);
	    if (steerInfo.weapon == NWEAPONS){
		steerInfo.weapon = (Weapon_t)0;
	    }
	    weaponState = HOLD;
	    break;
        default: break;
    }

    agent->setSteering(steerInfo);
}

PlayerController::PlayerController(Agent *agent)
                                  : turnState(STRAIGHT), engineState(NEUTRAL),
                                    weaponState(HOLD), agent(agent)
{
}

PlayerController::PlayerController()
                                  : turnState(STRAIGHT), engineState(NEUTRAL),
                                    agent(NULL)
{}
