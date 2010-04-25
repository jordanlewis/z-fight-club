#ifndef AGENT_H
#define AGENT_H

#include "Utilities/vec3f.h"
#include "kinematic.h"
#include <vector>
#include <iostream>

using namespace std;

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
    static float mass;     /* must be nonzero! */
    static float power;     /* p = m*v*a.  This and mass control accel rate  */
    static float maxRotate;    /* !<how fast we're allowed to rotate */
    static float height;
    static float width;
    static float depth;
    Kinematic kinematic;   /* !<car's current kinematic */
    std::vector<Vec3f> trail;   /* !<record of where the agent has been */

    Agent (Vec3f);      /* Constructor; Initial position */
    Agent (Vec3f, float); /* Constructor; Initial position and orientation */

    float getMaxAccel(); /* Calculate the current max acceleration */

    /* The below method is the interface between the generic agent
     * implemented in agent.c and the two types of agent controllers - AI
     * or Input. */

    void setKinematic (const Kinematic &kinematic); /* !< set agent's kinematic */
    Kinematic &getKinematic ();     /* !< get agent's kinematic */
    void setSteering (const SteerInfo &); /* !< Change desired steering */
    SteerInfo &getSteering ();      /* !< Change desired steering */
};

//! \brief Writes this agent to the given output stream.
//         This will be used for debugging.
std::ostream &operator<<(std::ostream&, const Agent&);

#endif
