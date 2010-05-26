#include "hud.h"
#include "Agents/agent.h"
#include "Engine/world.h"
#include "Agents/ai.h"
#include <SDL/SDL.h>
#include <cmath>
#include <sstream>
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif
extern "C" {
        #include "Utilities/load-png.h"
}

Widget::Widget(Vec3f pos) : pos(pos)
{}

static void drawText(Vec3f pos, string text, void *font)
{
    if (font == NULL)
        font = GLUT_BITMAP_HELVETICA_18;
    glRasterPos2f(pos.x, pos.y);
    for (unsigned int i = 0; i < text.size(); i++)
        glutBitmapCharacter(font, text[i]);
}

Speedometer::Speedometer(Vec3f pos, Agent *agent)
    : Widget(pos), agent(agent)
{
    World &world = World::getInstance();
    background = LoadImage((world.assetsDir + std::string("speedometer/background.png")).c_str(), false, RGBA_IMAGE);
}

void Speedometer::draw()
{
    World &w = World::getInstance();
    DrawImage(background,
       w.camera.getWres() - background->wid - pos[0],
       w.camera.getHres() - pos[0]);
    World &world = World::getInstance();

    glLineWidth(5);
    float theta = (agent->getKinematic().vel.length()*M_PI/15)-0.75;
    while(theta > (2*M_PI)) theta -= (2*M_PI);
    float center[2] = {105,105};

    glColor3f(1,0,0);
    glBegin(GL_LINES);
      glVertex2f(
          world.camera.getWres()-center[0],
          world.camera.getHres()-center[1]);
      glVertex2f(
          world.camera.getWres()-center[0]-(100*cos(theta)),
          world.camera.getHres()-center[1]-(100*sin(theta)));
    glEnd();
    glLineWidth(1);
}

LapCounter::LapCounter(Vec3f pos, Agent *agent)
    : Widget(pos), agent(agent)
{}

void LapCounter::draw()
{
    World &world = World::getInstance();
    int ypos = world.camera.getHres() - 10;
    stringstream ss;
    ss << "lap " << agent->lapCounter;
    glColor3f(0,0,1);
    drawText(Vec3f(10, ypos, 0), ss.str(), NULL);
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

Text::Text(Vec3f pos, string text) : Widget(pos), text(text)
{
}

void Text::draw()
{
    glRasterPos2f(pos.x, pos.y);
    for (unsigned int i = 0; i < text.size(); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
}

Menu::Menu(string name)
    : name(name)
{}

SubMenu::SubMenu(string name)
    : Menu(name)
{}

SubMenu::SubMenu(string name, list<Menu *> items)
    : Menu(name), items(items)
{}

void SubMenu::draw()
{
    World &world = World::getInstance();
    int hres = world.camera.getHres();
    int wres = world.camera.getWres();


    int hPos = 100;

    drawText(Vec3f(wres / 4, hPos, 0), name, GLUT_BITMAP_HELVETICA_18);

    for (list<Menu *>::iterator i = items.begin(); i != items.end(); i++) {
        hPos += 25;
        drawText(Vec3f(wres / 4, hPos, 0), (*i)->name, GLUT_BITMAP_HELVETICA_18);
    } 
}

TerminalMenu::TerminalMenu(string name, void (*callback)())
    : Menu(name), callback(callback)
{}

MiniMap::MiniMap(Vec3f pos, Path *path) : Widget(pos), path(path)
{}

void MiniMap::draw()
{
    World &world = World::getInstance();
    float yoff = world.camera.getHres() - 85;
    float xoff = 5;
    Vec3f vert;
    stringstream ss;

    glEnable(GL_BLEND);
    glColor4f(1,1,1,.4);
    glBegin(GL_QUADS);
    glVertex3f(0, world.camera.getHres(), 0);
    glVertex3f(250, world.camera.getHres(), 0);
    glVertex3f(250, world.camera.getHres() - 90, 0);
    glVertex3f(0, world.camera.getHres() - 90, 0);
    glEnd();
    glDisable(GL_BLEND);

    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glColor3f(1,0,1);
    for (unsigned int i = 0; i < path->knots.size(); i++)
    {
        vert = path->knots[i];
        glVertex2f(vert.x + xoff, vert.z + yoff);
    }
    glEnd();
    glLineWidth(1);

    glColor3f(0,0,1);
    for (unsigned int i = 0; i < world.wobjects.size(); i++)
    {
        if (!world.wobjects[i]->agent)
            continue;
        vert = world.wobjects[i]->agent->getKinematic().pos;
        vert.x += xoff;
        vert.y = vert.z + yoff;
        ss.seekp(0);
        ss << world.wobjects[i]->agent->id;
        drawText(vert, ss.str(), GLUT_BITMAP_HELVETICA_12);
    }
}

Places::Places(Vec3f pos) : Widget(pos)
{}

void Places::draw()
{
    AIManager &aim = AIManager::getInstance();
    World &world = World::getInstance();
    stringstream ss;
    ss << "places: ";
    for (unsigned int i = 0; i < aim.agentsSorted.size(); i++)
    {
        ss << aim.agentsSorted[i]->id << ", ";
    }
    glColor3f(0,0,1);
    drawText(Vec3f(75, world.camera.getHres() - 10, 0), ss.str(),
             GLUT_BITMAP_HELVETICA_18);
}
