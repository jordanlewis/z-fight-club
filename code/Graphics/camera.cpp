#include "camera.h"
#include "../Utilities/vec3f.h"
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
    FOVY = 65.0;

    pos = Vec3f(0.0f, 10.0f, 0.0f);
    up = Vec3f(1.0f, 0.0f, 0.0f);
    target = Vec3f(0.0f, 0.0f, 0.0f);

    wres = 800;
    hres = 600;
    zNear = 0.1f;
    zFar = 100.0f;
}

Camera::~Camera()
{
    // do nothing for now
}
        
void Camera::setTarget(Vec3f target)
{
    return;
}
void Camera::setProjectionMatrix()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);
    return;
}
