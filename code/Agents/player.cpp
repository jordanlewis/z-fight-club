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

void PlayerController::updateAgent()
{
    SteerInfo steerInfo;
    switch (engineState) {
        case NEUTRAL: steerInfo.acceleration = 0; break;
        case ACCELERATE: steerInfo.acceleration = agent.maxAccel; break;
        case REVERSE: steerInfo.acceleration = -agent.maxAccel; break;
        default: break;
    }
    switch (turnState) {
        case STRAIGHT: steerInfo.rotation = 0; break;
        case RIGHT:    steerInfo.rotation = -agent.maxRotate; break;
        case LEFT:     steerInfo.rotation = agent.maxRotate; break;
        default: break;
    }
    agent.setSteering(steerInfo);
}

PlayerController::PlayerController(Agent &agent)
                                  : turnState(STRAIGHT),
                                    engineState(NEUTRAL), agent(agent)
{
}
