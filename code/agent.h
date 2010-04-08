#ifndef AGENT_H
#define AGENT_H

#include "vector.h"

typedef enum {
    BRAKE = 0,
    ACCELERATE,
    NEUTRAL,
    REVERSE,
    nEngineState
} EngineState_t;

class Agent
{
    EngineState_t	engineState;	/* what the engine is doing right now */
    float 		turn_angle;     /* Angle of turn, 0 = no turn, radians */

    Rayf_t 		pos;		/* car's position and velocity */
    Vec3f_t 		orientation;	/* the direction the car is facing */

  public:
    Agent (Vec3f_t);  	/* Constructor; Initial position */
    ~Agent ();        /* Destructor */

    /* The below methods are the interface between the generic agent implemented
     * in agent.c and the two types of agent controllers - AI or Input. */

    void brake (bool);      /* Brake on or off? */
    void accelerate (bool); /* Acceleration on or off? */
    void reverse (bool);    /* Reverse on or off */
    void EngineState(EngineState_t); /* sets the engine State */
    void turn (float);      /* Turning? Angle of turn, 0 = no turn */
};

#endif
