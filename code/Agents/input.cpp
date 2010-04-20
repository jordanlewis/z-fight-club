#include "input.h"
#include "agent.h"

Input Input::_instance;

void Input::setTurnState(TurnState_t newState)
{
    turnState = newState;
}

void Input::setEngineState(EngineState_t newState)
{
    engineState = newState;
}

void Input::controlAgent(Agent *agent)
{
    this->agent = agent;
}

void Input::updateAgent()
{
    SteerInfo steerInfo;
    switch (engineState) {
        case NEUTRAL: steerInfo.acceleration = 0; break;
        case ACCELERATE: steerInfo.acceleration = agent->maxAccel; break;
        case REVERSE: steerInfo.acceleration = -agent->maxAccel; break;
        default: break;
    }
    switch (turnState) {
        case STRAIGHT: steerInfo.rotation = 0; break;
        case RIGHT:    steerInfo.rotation = agent->maxRotate; break;
        case LEFT:     steerInfo.rotation = -agent->maxRotate; break;
        default: break;
    }
    agent->setSteering(steerInfo);
}

Input &Input::getInstance()
{
    return _instance;
}

Input::Input()
{
}

Input::~Input()
{
}
