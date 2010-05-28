#include "hud.h"
#include "Agents/agent.h"
#include "Engine/world.h"
#include "Agents/ai.h"
#include "Utilities/error.h"
#include "Engine/scheduler.h"
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

SubMenu::SubMenu(string name, vector<Menu *> items)
    : Menu(name), items(items), highlighted(0), selected(-1) 
{
    /* set our submenu's to have us as their up menu */
    for (vector<Menu *>::iterator i = items.begin(); i != items.end(); i++) {
        SubMenu *subMenu = dynamic_cast<SubMenu *> (*i);
        if (subMenu)
            subMenu->parent = this;
    }
}

void SubMenu::draw()
{
    if (selected == -1) {
        /* we're in this level */
        World &world = World::getInstance();
        int hres = world.camera.getHres();
        int wres = world.camera.getWres();


        int hPos = 100;

        drawText(Vec3f(wres / 4, hPos, 0), name, GLUT_BITMAP_HELVETICA_18);

        glBegin(GL_LINES);
        glVertex3f((wres / 4), hPos + 25 * (highlighted + 1), 0);
        glVertex3f((wres / 4) + 100, hPos + 25 * (highlighted + 1), 0);
        glEnd();

        for (vector<Menu *>::iterator i = items.begin(); i != items.end(); i++) {
            hPos += 25;
            drawText(Vec3f(wres / 4, hPos, 0), (*i)->name, GLUT_BITMAP_HELVETICA_18);
        }
    }
    else {
        SubMenu *sub = dynamic_cast<SubMenu *>(items[selected]);
        if (sub)
            sub->draw();
        else {
            Error &error = Error::getInstance();
            error.log(GRAPHICS, CRITICAL, "Menu selected is of a TerminalItem, this is illegal\n");
            exit(0);
        }
    }
}

void SubMenu::highlightNext()
{
    if (selected == -1) {
        highlighted++;
        highlighted = highlighted % items.size();
    } else {
        SubMenu *sub = dynamic_cast<SubMenu *>(items[selected]);
        if (sub)
            sub->highlightNext();
        else {
            Error &error = Error::getInstance();
            error.log(GRAPHICS, CRITICAL, "Menu selected is of a TerminalItem, this is illegal\n");
            exit(0);
        }
    }
}

void SubMenu::highlightPrev()
{
    if (selected == -1) {
        highlighted--;
        highlighted = highlighted % items.size();
    } else {
        SubMenu *sub = dynamic_cast<SubMenu *>(items[selected]);
        if (sub)
            sub->highlightPrev();
        else {
            Error &error = Error::getInstance();
            error.log(GRAPHICS, CRITICAL, "Menu selected is of a TerminalItem, this is illegal\n");
            exit(0);
        }
    }
}

void SubMenu::select()
{
    if (selected == -1) {
        TerminalMenu *term = dynamic_cast<TerminalMenu *>(items[highlighted]);
        if (term)
            term->select();
        else 
            selected = highlighted;
    }
    else {
        SubMenu *sub = dynamic_cast<SubMenu *>(items[selected]);
        if (sub)
            sub->select();
        else {
            Error &error = Error::getInstance();
            error.log(GRAPHICS, CRITICAL, "Menu selected is of a TerminalItem, this is illegal\n");
            exit(0);
        }
    }
}


bool SubMenu::up()
{
    if (selected == -1) {
        if (parent == NULL)
            Scheduler::getInstance().raceState = RACE;
        return true;
    } else {
        SubMenu *sub = dynamic_cast<SubMenu *>(items[selected]);
        if (sub) {
            if (sub->up()) {
                /* getting here means our child was the node level the user was on */
                selected = -1;
            }
        }
        else {
            Error &error = Error::getInstance();
            error.log(GRAPHICS, CRITICAL, "Menu selected is of a Terminal menu, this is illegal\n");
            exit(0);
        }
    }
}

void SubMenu::reset()
{
    selected = -1;
    highlighted = 0;
    for (vector<Menu *>::iterator i = items.begin(); i != items.end(); i++) {
        /* reset our submenus */
        SubMenu *sub = dynamic_cast<SubMenu *>(*i);
        if (sub)
            sub->reset();
    }
}

TerminalMenu::TerminalMenu(string name, void (*callback)())
    : Menu(name), callback(callback)
{}

void TerminalMenu::select()
{
    (*callback)();
}

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
