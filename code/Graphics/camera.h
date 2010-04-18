#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities/vector.h"

class Camera {
    /* The position, orientation, and up vector for the Camera.
       NOTE: we may again use quaternions for this, or represent it in spherical
             coordinates
    */
    Vec3f_t position,
            up,
            target;
  public:
    Camera();
    ~Camera();
    
    void SetTarget(Vec3f_t target);
    void SetProjectionMatrix(); //Maybe this should be protected, and we call
                                //graphics a friend class, then it won't be
                                //visible to people that don't need it.
};

#endif
