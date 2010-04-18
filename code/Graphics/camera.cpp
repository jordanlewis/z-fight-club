#include "camera.h"
#include <SDL/SDL.h>

#if defined (__APPLE__) && defined (__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

Camera::Camera()
{
    // do nothing for now
}

Camera::~Camera()
{
    // do nothing for now
}
        
void Camera::setTarget(Vec3f_t target)
{
    return;
}
void Camera::setProjectionMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);
    return;
}
