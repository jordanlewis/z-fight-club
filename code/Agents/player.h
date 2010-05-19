#ifndef PLAYER_H
#define PLAYER_H

#include "agent.h"
struct RPPlayerControl;
#include <iostream>

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

typedef enum {
    FIRE = 0,
    HOLD,
    CHANGE
} WeaponState_t;

class PlayerController
{
    TurnState_t turnState;
    EngineState_t engineState;
    WeaponState_t weaponState;
    Agent *agent;

  public:
    void setTurnState(TurnState_t newState);
    void setEngineState(EngineState_t newState);
    void setWeaponState(WeaponState_t newState);

    const TurnState_t &getTurnState();
    const EngineState_t &getEngineState();
    const WeaponState_t &getWeaponState();

    const Agent &getAgent() const;
    void updateAgent();

    PlayerController(Agent *agent);
    PlayerController();
    void hton(RPPlayerControl *payload);
    void ntoh(RPPlayerControl *payload);

    //! \brief Writes this player controller to the given output stream.
    //         This will be used for debugging.
    friend std::ostream &operator<<(std::ostream&, const PlayerController&);
};

#endif
