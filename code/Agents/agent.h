#ifndef AGENT_H
#define AGENT_H

#include "Utilities/vec3f.h"
#include <vector>

class Kinematic
{
  public:
    Vec3f pos;
    Vec3f vel;
    Vec3f orientation_v;

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
    SteerInfo steerInfo;   /* !<car's steering info, set by AI/human */

  public:
    unsigned int id;       /* !<internal id number */
    static float maxAccel;     /* !<how fast we're allowed to accelerate */
    static float maxRotate;    /* !<how fast we're allowed to rotate */
    static float height;
    static float width;
    static float depth;
    Kinematic kinematic;   /* !<car's current kinematic */

    Agent (Vec3f);      /* Constructor; Initial position */
    Agent (Vec3f, float); /* Constructor; Initial position and orientation */

    /* The below method is the interface between the generic agent
     * implemented in agent.c and the two types of agent controllers - AI
     * or Input. */

    void setKinematic (const Kinematic &kinematic); /* !< set agent's kinematic */
    Kinematic &getKinematic ();     /* !< get agent's kinematic */
    void setSteering (const SteerInfo &); /* !< Change desired steering */
    SteerInfo &getSteering ();      /* !< Change desired steering */
};

#endif
