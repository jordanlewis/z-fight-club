#include "hud.h"
#include <SDL/SDL.h>
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif


Speedometer::Speedometer(Vec3f pos, Agent *agent)
    : Widget(), agent(agent)
{
    this->pos = pos;
}

Speedometer::~Speedometer()
{}

void Speedometer::draw()
{
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(350.0f, 350.0f);
    glEnd();
}

Hud Hud::_instance;

Hud::Hud()
{}

Hud::~Hud()
{}

Hud &Hud::getInstance()
{
    return _instance;
}
