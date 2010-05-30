#ifndef AGENT_H
#define AGENT_H

#include "allclasses.h"
#include "kinematic.h"
#include "constants.h"
#include <vector>
#include <list>
#include <iostream>
#include <stdint.h>

using namespace std;

typedef enum {
    NONE = 0,
    SMACK,
    RAYGUN,
    ROCKET,
    MINE,
    NWEAPONS
} Weapon_t;

class SteerInfo
{
  public:
    float acceleration;
    float rotation;
    Weapon_t weapon;
    int fire;
    SteerInfo();
    void hton(RPSteerInfo *);
    void ntoh(const RPSteerInfo *);
};

//! \brief Writes this SteerInfo to the given output stream.
//         This will be used for debugging.
std::ostream &operator<<(std::ostream&, const SteerInfo&);

class WorldObject;
struct RPAgent;

class Agent
{
    static uint32_t maxId; /* !<highest id number we've reached */

  public:
    Agent();
    Agent(Vec3f);      /* Constructor; Initial position */
    Agent(Vec3f, float); /* Constructor; Initial position and orientation */

    SteerInfo steerInfo;   /* !<car's steering info, set by AI/human */
    WorldObject *worldObject; /* !<back pointer to world object */
    uint32_t id;
    static float mass;     /* must be nonzero! */
    static float power;     /* p = m*v*a.  This and mass control accel rate  */
    static float maxRotate;    /* !<how fast we're allowed to rotate */
    static float height;
    static float width;
    static float depth;
    Kinematic kinematic;   /* !<car's current kinematic */
    std::vector<Vec3f> trail;   /* !<record of where the agent has been */

    float getMaxAccel(); /* Calculate the current max acceleration */

    unsigned int pathPosition; /*!< index into world path */
    unsigned int lapCounter;   /*!< how many laps have we completed */
    unsigned int pathDistance; /*!< distance around track */

    void hton(RPAgent *payload); /* Package the agent for network transfer */
    void ntoh(RPAgent *payload); /* Unpackage an agent from network transfer */

    void resetToTrack(); /* Reset agent onto track */
    /* The below method is the interface between the generic agent
     * implemented in agent.c and the two types of agent controllers - AI
     * or Input. */

    void setKinematic (const Kinematic &kinematic); /* !< set agent's kinematic */
    Kinematic &getKinematic ();     /* !< get agent's kinematic */
    const Kinematic &getKinematic () const;     /* !< get const agent's kinematic */
    void setSteering (const SteerInfo &); /* !< Change desired steering */
    SteerInfo &getSteering ();      /* !< Get current steering */
};

//! \brief Writes this agent to the given output stream.
//         This will be used for debugging.
std::ostream &operator<<(std::ostream&, const Agent&);

#endif
