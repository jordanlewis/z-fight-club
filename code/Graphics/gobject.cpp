#include "gobject.h"
#include "Engine/geominfo.h"
#include "Agents/agent.h"
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

GObject::GObject(GeomInfo *geominfo) : geominfo(geominfo),agentBank(0)
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
    Quatf_t newquat;
    CopyQuatf(quat, newquat);
    float heading, bank, attitude;

    /* quat -> euler -> quat math from
     * http://www.euclideanspace.com/maths/geometry/rotations/conversions
     */
    heading  = atan2(2 * quat[1] * quat[3] - 2 * quat[0] * quat[2],
                     1 - 2 * quat[1] * quat[1] - 2 * quat[2] * quat[2]);
    attitude =  asin(2 * quat[0] * quat[1] + 2 * quat[2] * quat[3]);
    bank     = atan2(2 * quat[0] * quat[3] - 2 * quat[1] * quat[2],
                     1 - 2 * quat[0] * quat[0] - 2 * quat[2] * quat[2]);


    /* North pole */
    if (quat[0] * quat[1] + quat[2] * quat[3] == .5)
    {
        heading = 2 * atan2(quat[0], quat[3]);
        bank = 0;
    }

    /* south pole */
    if (quat[0] * quat[1] + quat[2] * quat[3] == -.5)
    {
        heading = -2 * atan2(quat[0], quat[3]);
        bank = 0;
    }

    agentBank = .99 * agentBank + .01 * M_PI_4 * -s.rotation;
    //bank = M_PI_4 * -s.rotation;
    //attitude = 0;
    bank = agentBank;

    float c1, c2, c3, s1, s2, s3;
    c1 = cos(heading / 2);
    c3 = cos(attitude / 2);
    c2 = cos(bank / 2);
    s1 = sin(heading / 2);
    s3 = sin(attitude / 2);
    s2 = sin(bank / 2);

    newquat[0] = s1 * s2 * c3 + c1 * c2 * s3; // x
    newquat[1] = s1 * c2 * c3 + c1 * s2 * s3; // y
    newquat[2] = c1 * s2 * c3 - s1 * c2 * s3; // z
    newquat[3] = c1 * c2 * c3 - s1 * s2 * s3; // w
    draw(pos, newquat);
}
