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
<<<<<<< HEAD
=======
    static float maxAccel;     /* !<how fast we're allowed to accelerate */
    static float maxRotate;    /* !<how fast we're allowed to rotate */

>>>>>>> 20766d157dd67b280079459bc931e00927bb47a0
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

    void getKinematic (Kinematic *kinematic); /* !< get agent's kinematic */
    void setSteering (SteerInfo &); /* Change desired steering */
};

#endif
