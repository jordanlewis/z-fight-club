#include "camera.h"
#include "Utilities/error.h"
#include "Agents/agent.h"
#include "allclasses.h"
#include "Engine/world.h"
#include <SDL/SDL.h>

#if defined (__APPLE__) && defined (__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

Camera::Camera() :
    error(&Error::getInstance())
{
    agent = NULL;
    mode = OVERHEAD;


    setup();
}

Camera::Camera(CameraMode_t mode, Agent *agent) : error(&Error::getInstance())
{
    this->agent = agent;
    this->mode = mode;

    smooth_orientation = agent->kinematic.orientation_v;
    setup();
}

void Camera::setup()
{
    FOVY = 65.0;
    pos = Vec3f(100.0f, 20.0f, 50.0f);
    up = Vec3f(0.0f, 1.0f, 0.0f);
    target = Vec3f(0.0f, 0.0f, 0.0f);

    World &world = World::getInstance();
    if (world.fullscreen)
    {
        hres = world.hres;
        wres = world.wres;
    }
    else
    {
        wres = 800;
        hres = 600;
    }
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

void Camera::cycleView()
{
    if (agent == NULL)
    {
        mode = OVERHEAD;
        return;
    }
        
    if (mode < nModes - 1)
        mode = static_cast<CameraMode_t>(static_cast<int>(mode) + 1);
    else
        mode = static_cast<CameraMode_t>(0);
}

void Camera::setProjectionMatrix()
{
    SteerInfo s;
    float minfovy = 55.0;
    float maxfovy = 90;
    Vec3f dolly;

    float smoothness = .9;

    switch (mode) {
        case FIRSTPERSON: smoothness = 0; break;
        case THIRDPERSON: smoothness = .9; break;
        case BIRDSEYE:    smoothness = .99; break;
        default: break;
    }
    if (agent != NULL)
        smooth_orientation = smoothness * smooth_orientation +
                             (1 - smoothness) * agent->kinematic.orientation_v;

    // This actually gets modified if we're using glow, so don't set it here
    //glViewport(0, 0, wres, hres);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    switch (mode) {
        case OVERHEAD:
            pos = Vec3f(44.0f, 70.0f, 30.0f);
            up = Vec3f(1.0f, 0.0f, 0.0f);
            target = Vec3f(44.0f, 0.0f, 30.0f);
            break;
        case FIRSTPERSON:
            if (agent == NULL) {
                error->log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
                exit(0);
            }
            pos = agent->kinematic.pos;
            up = Vec3f(0.0f, 1.0f, 0.0f);
            target = (agent->kinematic.pos + smooth_orientation);
            break;
        case THIRDPERSON:
            if (agent == NULL) {
                error->log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
                exit(0);
            }
            pos = (agent->kinematic.pos - (1 * smooth_orientation) + Vec3f(0.0f, 0.8f, 0.0f));
            up = Vec3f(0.0f, 1.0f, 0.0f);
            target = (agent->kinematic.pos + (5 * smooth_orientation));
            s = agent->getSteering();
            if (s.acceleration > 0 && agent->kinematic.forwardSpeed() > 0 &&
                FOVY < maxfovy)
            {
                FOVY += ((maxfovy - FOVY)/(maxfovy - minfovy))/2;
            }
            else if (s.acceleration <= 0 && FOVY > minfovy)
            {
                FOVY -= ((FOVY - minfovy)/(maxfovy - minfovy))/2;
            }
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective((GLdouble) FOVY, (GLdouble) wres / (GLdouble) hres, zNear, zFar);

            glMatrixMode(GL_MODELVIEW);

            dolly = (pos - agent->kinematic.pos);
            /* Dolly = old dolly scaled properly for dollyzoom with respect to
             * FOVY, plus an extra length corresponding to how zoomed out we
             * are. the extra length makes it so the dolly zoom is slightly
             * laggy - i.e. it doesn't dolly quite enough to match the zoom.
             */
            dolly = dolly.length() * dolly.unit() *
                    tan(toRads(minfovy/2)) / tan(toRads(FOVY/2)) +
                    dolly.unit() * .05 * (FOVY - minfovy);
            pos = agent->kinematic.pos + dolly;

            break;
        case BIRDSEYE:
            if (agent == NULL) {
                error->log(GRAPHICS, CRITICAL, "Agent in camera not set, but agent specific mode selected\n");
                exit(0);
            }
            pos = agent->kinematic.pos + Vec3f(0.0f, 30.0f, 0.0f);
            up = smooth_orientation;
            up.y = 0.0f;
            up.normalize();
            target = agent->kinematic.pos;
            break;
        default: break;
    }
    gluLookAt(pos[0], pos[1], pos[2], target[0], target[1], target[2], up[0], up[1], up[2]);
    return;
}

void Camera::setOrthoMatrix()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (GLdouble) wres, hres, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float Camera::getWres()
{
    return wres;
}
float Camera::getHres()
{
    return hres;
}

const Vec3f &Camera::getTarget() const
{
    return target;
}

const Vec3f &Camera::getPos() const
{
    return pos;
}

const Vec3f &Camera::getUp() const
{
    return up;
}

const Agent *Camera::getAgent() const
{
    return agent;
}
