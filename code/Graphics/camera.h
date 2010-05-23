#ifndef CAMERA_H
#define CAMERA_H

#include "allclasses.h"
#include "Utilities/vec3f.h"

typedef enum {
    OVERHEAD = 0,       /* !< overhead view on the world */
    FIRSTPERSON,        /* !< view from an agent first person */
    THIRDPERSON,        /* !< view from behind the agent */
    BIRDSEYE,           /* !< above the agent, but follow */
    nModes
} CameraMode_t;

class Camera {
    CameraMode_t        mode;   /* !< what mode the camera is in */
    Agent               *agent; /* !< the agent this camera is follow (can be null) */
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
    Error *error;
  public:
    Camera();
    Camera(CameraMode_t, Agent *);
    ~Camera();

    void setTarget(Vec3f target);
    void cycleView(); /* !< cycle camera views */
    void setProjectionMatrix(); //Maybe this should be protected, and we call
    void setOrthoMatrix(); //for 2d graphics
    const Vec3f &getTarget() const;   //graphics a friend class, then it won't be
    const Vec3f &getPos() const;      //visible to people that don't need it.
    const Vec3f &getUp() const;
    const Agent *getAgent() const;

    float getWres();
    float getHres();

    friend class Graphics;
    friend class Input;
};

#endif
