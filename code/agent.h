#ifndef AGENT_H
#define AGENT_H

#include "vector.h"
#include <vector>

class SteerInfo
{
  public:
    float acceleration;
    float rotation;
};

class Agent
{
    static unsigned int maxId; /* !<highest id number we've reached */
  public:
    unsigned int id;       /* !<internal id number */
    SteerInfo steerInfo;   /* !<car's steering info, set by AI/human */
    Rayf_t    pos;         /* !<car's position and velocity */
    float     orientation; /* !<the direction the car is facing, radians */

    Agent (Vec3f_t);      /* Constructor; Initial position */
    Agent (Vec3f_t, float); /* Constructor; Initial position and orientation */
    ~Agent ();        /* Destructor */

    /* The below method is the interface between the generic agent
        * implemented in agent.c and the two types of agent controllers - AI
        * or Input. */

    void SetSteering (SteerInfo &); /* Change desired steering */
};

#endif
