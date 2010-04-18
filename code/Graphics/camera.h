#ifndef CAMERA_H
#define CAMERA_H

#include "../Utilities/vector.h"

class Camera {
    /* The position, orientation, and up vector for the Camera.
       NOTE: we may again use quaternions for this, or represent it in spherical
             coordinates
    */
    float FOVY;

    Vec3f_t pos,
            up,
            target;

    int	wres,
	hres;

    float zNear,
	  zFar;
  public:
    Camera();
    ~Camera();
    
    void setTarget(Vec3f_t target);
    void setProjectionMatrix(); //Maybe this should be protected, and we call
                                //graphics a friend class, then it won't be
                                //visible to people that don't need it.
};

#endif
