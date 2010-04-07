#ifndef AGENT_H
#define AGENT_H

#include "vector.h"

class Agent
{
    bool is_braking;       /* Brake on or off? */
    bool is_accelerating;  /* Accelerating or not? */
    float turn_angle;      /* Angle of turn, 0 = no turn, radians */

    Rayf_t pos;		   /* car's position and velocity */
    float velocity;	   /* I think we need this so that cars at rest still have am orientation */

  public:
    Agent (Vec3f_t);  /* Constructor; Initial position */
    ~Agent ();        /* Destructor */

    /* The below methods are the interface between the generic agent implemented
     * in agent.c and the two types of agent controllers - AI or Input. */

    void brake (bool);      /* Brake on or off? */
    void accelerate (bool); /* Acceleration on or off? */
    void turn (float);      /* Turning? Angle of turn, 0 = no turn */
}

#endif
