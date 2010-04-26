#ifndef PLAYER_H
#define PLAYER_H

#include "agent.h"

typedef enum {
    BRAKE = 0,
    ACCELERATE,
    NEUTRAL,
    REVERSE,
    nEngineState
} EngineState_t;

typedef enum {
    STRAIGHT = 0,
    LEFT = 1,
    RIGHT = 2
} TurnState_t;

class PlayerController
{
    TurnState_t turnState;
    EngineState_t engineState;
    Agent *agent;

  public:
    void setTurnState(TurnState_t newState);
    void setEngineState(EngineState_t newState);

    const TurnState_t &getTurnState();
    const EngineState_t &getEngineState();

    void updateAgent();

    PlayerController(Agent &agent);
    PlayerController();
};

#endif
