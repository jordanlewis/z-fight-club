#ifndef AGENT_H
#define AGENT_H

#include "vector.h"
#include <vector>

namespace Agent
{
    /* \brief Class Path
     * \brief a path for an agent to follow
     */
    class Path
    {
        std::vector<Vec3f_t>        knots;        /* !< knots defining the path */
        std::vector<float>        precision;    /* !< how closely we want to follow the knots */

    public:
        Path ();
        Path (std::vector<Vec3f_t>);
        ~Path();

        Path *PathToPath(Vec3f_t position, float urgency);
    };

    typedef enum {
        BRAKE = 0,
        ACCELERATE,
        NEUTRAL,
        REVERSE,
        nEngineState
    } EngineState_t;

    class Agent
    {
        EngineState_t    engineState;    /* !<what the engine is doing right now */
        float         turn_angle;     /* !<Angle of turn, 0 = no turn, radians */
    
        Rayf_t         pos;        /* !<car's position and velocity */
        Vec3f_t     orientation;    /* !<the direction the car is facing */
    
        std::vector<Path>    pathStack;    /* !<paths for the agent to follow */
    
      public:
        Agent (Vec3f_t);      /* Constructor; Initial position */
        Agent (Vec3f_t, Vec3f_t); /* Constructor; Initial position and orientation */
        ~Agent ();        /* Destructor */
    
        /* The below methods are the interface between the generic agent implemented
         * in agent.c and the two types of agent controllers - AI or Input. */
    
        void EngineState(EngineState_t); /* sets the engine State */
        void turn (float);      /* Turning? Angle of turn, 0 = no turn */
    };


}

#endif
