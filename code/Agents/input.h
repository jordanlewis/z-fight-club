#ifndef INPUT_H
#define INPUT_H

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

class Input
{
    TurnState_t turnState;
    EngineState_t engineState;

    static Input _instance;
    Agent *agent;

  public:
    void setTurnState(TurnState_t newState);
    void setEngineState(EngineState_t newState);

    void controlAgent(Agent *agent);
    void updateAgent();

    static Input &getInstance();
};


#endif
