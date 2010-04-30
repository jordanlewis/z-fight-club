#include "camera.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"
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
    agent = NULL;
    mode = OVERHEAD;

    FOVY = 65.0;

    pos = Vec3f(100.0f, 20.0f, 50.0f);
    up = Vec3f(0.0f, 1.0f, 0.0f);
    target = Vec3f(0.0f, 0.0f, 0.0f);

    wres = 800;
    hres = 600;
    zNear = 0.1f;
    zFar = 1000.0f;
}

Camera::Camera(CameraMode_t mode, Agent *agent)
{
    this->agent = agent;
    this->mode = mode;

    FOVY = 65.0;

    smooth_orientation = agent->kinematic.orientation_v;
    pos = Vec3f(100.0f, 20.0f, 50.0f);
    up = Vec3f(0.0f, 1.0f, 0.0f);
    target = Vec3f(0.0f, 0.0f, 0.0f);

    wres = 800;
    hres = 600;
    zNear = 0.1f;
    zFar = 1000.0f;
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
    Error error = Error::getInstance();
    SteerInfo s;
    float minfovy = 55.0;
    float maxfovy = 90;
    Vec3f dolly;

    if (agent != NULL)
        smooth_orientation = .9 * smooth_orientation + .1 * agent->kinematic.orientation_v;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    switch (mode) {
	case OVERHEAD:
	    pos = Vec3f(100.0f, 20.0f, 50.0f);
	    up = Vec3f(0.0f, 1.0f, 0.0f);
	    target = Vec3f(0.0f, 0.0f, 0.0f);
	    break;
	case FIRSTPERSON:
	    if (agent == NULL) {
		error.log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
		exit(0);
	    }
	    pos = agent->kinematic.pos;
	    up = Vec3f(0.0f, 1.0f, 0.0f);
	    target = (agent->kinematic.pos + smooth_orientation);
	    break;
	case THIRDPERSON:
	    if (agent == NULL) {
		error.log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
		exit(0);
	    }
	    pos = (agent->kinematic.pos - (5 * smooth_orientation) + Vec3f(0.0f, 3.0f, 0.0f));
	    up = Vec3f(0.0f, 1.0f, 0.0f);
	    target = (agent->kinematic.pos + (5 * smooth_orientation));
            s = agent->getSteering();
            if (s.acceleration > 0)
                FOVY += FOVY <= maxfovy ? .25 : 0;
            else
                FOVY -= FOVY >= minfovy ? .25 : 0;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);

            glMatrixMode(GL_MODELVIEW);

        dolly = (pos - agent->kinematic.pos);
        dolly = ( (dolly.length() * tan(toRads(minfovy/2)))/tan(toRads(FOVY/2)) ) * dolly.unit();
        pos = agent->kinematic.pos + dolly;

	    break;
	case BIRDSEYE:
	    if (agent == NULL) {
		error.log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
		exit(0);
	    }
	    pos = agent->kinematic.pos + Vec3f(0.0f, 60.0f, 0.0f);
	    up = smooth_orientation;
	    up.y = 0.0f;
	    up.normalize();
	    target = agent->kinematic.pos;
	    break;
    }
    gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
    return;
}


const Vec3f &Camera::getTarget()
{
    return target;
}

const Vec3f &Camera::getPos()
{
    return pos;
}
