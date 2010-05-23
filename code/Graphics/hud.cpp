#include "hud.h"
#include "Agents/agent.h"
#include "Engine/world.h"
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
    World &world = World::getInstance();

    glLineWidth(5);
    if (agent->getKinematic().forwardSpeed() > 0)
        glColor3f(0,1,0);
    else
        glColor3f(1,0,0);

    glBegin(GL_LINES);
    glVertex2f(20, world.camera.getHres() - 20);
    glVertex2f(20, world.camera.getHres() - (20 + agent->getKinematic().vel.length() * 20));
    glEnd();
    glLineWidth(1);
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
