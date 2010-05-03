#include "gobject.h"
#include "Engine/geominfo.h"
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

GObject::GObject(GeomInfo *geominfo) : geominfo(geominfo)
{}

void GObject::draw(Vec3f pos, Quatf_t quat)
{
    Mat4x4f_t matrix;
    ToRotMatrix(quat, matrix);

    matrix[12]=pos[0];
    matrix[13]=pos[1];
    matrix[14]=pos[2];

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(matrix);

    geominfo->draw();

    glPopMatrix();
}

void GObject::draw(Vec3f pos, Quatf_t quat, Agent* agent)
{
    SteerInfo s = agent->getSteering();
    if (s.acceleration > 0)
    {
        glColor3f(.3, 1, .3);
    }
    else if (s.acceleration < 0)
    {
        glColor3f(1, .3, .3);
    }
    else
    {
        glColor3f(1,1,1);
    }
    draw(pos, quat);
}
