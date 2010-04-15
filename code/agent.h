#ifndef AGENT_H
#define AGENT_H

#include "vector.h"
#include <vector>

class Kinematic
{
  public:
    Vec3f_t pos;
    Vec3f_t vel;

    float orientation;
};

class SteerInfo
{
  public:
    float acceleration;
    float rotation;
};

class Agent
{
    static unsigned int maxId; /* !<highest id number we've reached */
    static float maxAccel;     /* !<how fast we're allowed to accelerate */
    static float maxRotate;    /* !<how fast we're allowed to rotate */

    Kinematic kinematic;   /* !<car's current kinematic */
    SteerInfo steerInfo;   /* !<car's steering info, set by AI/human */

  public:
    unsigned int id;       /* !<internal id number */
    static float height;
    static float width;
    static float depth;

    Agent (Vec3f_t);      /* Constructor; Initial position */
    Agent (Vec3f_t, float); /* Constructor; Initial position and orientation */

    /* The below method is the interface between the generic agent
     * implemented in agent.c and the two types of agent controllers - AI
     * or Input. */

    void setKinematic (Kinematic &kinematic); /* !< set agent's kinematic */
    Kinematic &getKinematic ();     /* !< get agent's kinematic */
    void setSteering (SteerInfo &); /* !< Change desired steering */
    SteerInfo &getSteering ();      /* !< Change desired steering */
};

#endif
