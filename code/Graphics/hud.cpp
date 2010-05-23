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
extern "C" {
        #include "Utilities/load-png.h"
}

Widget::Widget(Vec3f pos) : pos(pos)
{}

Speedometer::Speedometer(Vec3f pos, Agent *agent)
    : Widget(pos), agent(agent)
{
    World &world = World::getInstance();
    background = LoadImage((world.assetsDir + std::string("speedometer/background.png")).c_str(), false, RGBA_IMAGE);
}

void Speedometer::draw()
{
    DrawImage(background, pos[0], pos[1]);
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

LapCounter::LapCounter(Vec3f pos, Agent *agent)
    : Widget(pos), agent(agent)
{}

void LapCounter::draw()
{
    World &world = World::getInstance();
    int hpos = world.camera.getHres() - 20;
    glColor3f(0,0,1);
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < agent->lapCounter; i++)
    {
        glVertex2f(50 + 30*i, hpos);
        glVertex2f(40 + 30*i, hpos - 20);
        glVertex2f(30 + 30*i, hpos);
    }
    glEnd();

}

StopLight::StopLight(Vec3f pos) : Widget(pos), nLit(0)
{}

void StopLight::draw()
{
    World &world = World::getInstance();
    float xpos = world.camera.getWres() / 2.0;
    float ypos = world.camera.getHres() / 2.0;

    glColor3f(1,0,0);
    for (int i = 0; i < 3; i++)
    {
        switch (i)
        {
            case 0: glColor3f(1,0,0); break;
            case 1: glColor3f(1,1,0); break;
            case 2: glColor3f(0,1,0); break;
        }
        if (i >= nLit)
            glBegin(GL_LINE_LOOP);
        else
            glBegin(GL_QUADS);

        glVertex2f(xpos + 10, ypos + i * 25);
        glVertex2f(xpos - 10, ypos + i * 25);
        glVertex2f(xpos - 10, ypos + 20 + i * 25);
        glVertex2f(xpos + 10, ypos + 20 + i * 25);
        glEnd();
    }
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
