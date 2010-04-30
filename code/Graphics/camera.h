#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities/vec3f.h"
#include "Agents/agent.h"

typedef enum {
    OVERHEAD = 0,	/* !< overhead view on the world */
    FIRSTPERSON,	/* !< view from an agent first person */
    THIRDPERSON,	/* !< view from behind the agent */
    BIRDSEYE		/* !< above the agent, but follow */
} CameraMode_t;

class Camera {
    CameraMode_t	mode;	/* !< what mode the camera is in */
    Agent		*agent;	/* !< the agent this camera is follow (can be null) */
    /* The position, orientation, and up vector for the Camera.
       NOTE: we may again use quaternions for this, or represent it in spherical
             coordinates
    */
    float FOVY;
    Vec3f smooth_orientation;

    Vec3f pos,
          up,
          target;

    int wres,
        hres;

    float zNear,
          zFar;
  public:
    Camera();
    Camera(CameraMode_t, Agent *);
    ~Camera();
    
    void setTarget(Vec3f target);
    void setProjectionMatrix(); //Maybe this should be protected, and we call
    const Vec3f &getTarget();   //graphics a friend class, then it won't be
    const Vec3f &getPos();      //visible to people that don't need it.
                                
};

#endif
