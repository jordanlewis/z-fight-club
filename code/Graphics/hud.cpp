#include "hud.h"
#include "Agents/agent.h"
#include "Engine/world.h"
#include "Agents/ai.h"
#include "Utilities/error.h"
#include "Engine/scheduler.h"
#include "Graphics/graphics.h"
#include "Physics/physics.h"
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
    : Menu(name), highlighted(0), selected(-1), items(items)
{
    /* set our submenu's to have us as their up menu */
    for (vector<Menu *>::iterator i = items.begin(); i != items.end(); i++) {
        SubMenu *subMenu = dynamic_cast<SubMenu *> (*i);
        TextboxMenu *text = dynamic_cast<TextboxMenu *> (*i);
        if (subMenu)
            subMenu->parent = this;
        if (text)
            text->parent = this;
    }
}

void SubMenu::draw()
{
    glColor3f(1.0, 1.0, 1.0);
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
        items[selected]->draw();
        
    }
}

void SubMenu::highlightNext()
{
    if (selected == -1) {
        highlighted++;
        if (highlighted >= items.size())
            highlighted = 0;
    } else {
        items[selected]->highlightNext();
    }
}

void SubMenu::highlightPrev()
{
    if (selected == -1) {
        highlighted--;
        if (highlighted < 0)
            highlighted = items.size() - 1;
    } else {
        items[selected]->highlightPrev();
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
        items[selected]->select();
    }
}


bool SubMenu::up()
{
    if (selected == -1) {
        if (parent == NULL)
            Scheduler::getInstance().raceState = RACE;
        return true;
    } else {
        if (items[selected]->up()) {
            /* getting here means our child was the node level the user was on */
            selected = -1;
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

void SubMenu::inputChar(char c)
{
    if (selected != -1)
        items[selected]->inputChar(c);
}

void SubMenu::backspace()
{
    if (selected != -1)
        items[selected]->backspace();
}

TerminalMenu::TerminalMenu(string name, void (*callback)())
    : Menu(name), callback(callback)
{}

void TerminalMenu::draw()
{
    Error &error = Error::getInstance();
    error.log(GRAPHICS, CRITICAL, "Menu selected is of a TerminalItem, this is illegal\n");
    exit(0);
}

void TerminalMenu::select()
{
    (*callback)();
}

void TerminalMenu::inputChar(char c)
{
    Error &error = Error::getInstance();
    error.log(GRAPHICS, CRITICAL, "Menu selected is of a Terminal menu, this is illegal\n");
    exit(0);
}

void TerminalMenu::backspace()
{
    Error &error = Error::getInstance();
    error.log(GRAPHICS, CRITICAL, "Menu selected is of a Terminal menu, this is illegal\n");
    exit(0);
}


TextboxMenu::TextboxMenu(string name)
    : Menu(name)
{}

void TextboxMenu::draw()
{
    World &world = World::getInstance();
    int hres = world.camera.getHres();
    int wres = world.camera.getWres();

    int hPos = 100;

    drawText(Vec3f(wres / 4, hPos, 0), name, GLUT_BITMAP_HELVETICA_18);

    glBegin(GL_LINE_LOOP);
    glVertex3f((wres / 4), hPos + 25, 0);
    glVertex3f((wres / 4) + 300, hPos + 25, 0);
    glVertex3f((wres / 4) + 300, hPos + 50, 0);
    glVertex3f((wres / 4), hPos + 50, 0);
    glEnd();

    drawText(Vec3f(wres / 4, hPos + 50, 0), entered, GLUT_BITMAP_HELVETICA_18);
}

void TextboxMenu::select()
{
    SubMenu *parent = dynamic_cast<SubMenu *>(parent);
    if(parent) {
        parent->selected = -1;
    }
}

void TextboxMenu::inputChar(char c)
{
    entered.append(1, c);
}

void TextboxMenu::backspace()
{
    if (!entered.empty())
        entered.erase(--(entered.end()));
}

SelectorMenu::SelectorMenu(string name, vector<Option *> options)
    : Menu(name), options(options)
{}

void SelectorMenu::draw()
{
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

    if (selected != -1) {
        glBegin(GL_LINE_LOOP);
        glVertex3f((wres / 4), hPos + 27 * (selected + 1), 0);
        glVertex3f((wres / 4) + 300, hPos + (27 * (selected + 1)), 0);
        glVertex3f((wres / 4) + 300, hPos + (25 * selected), 0);
        glVertex3f((wres / 4), hPos + (25 * selected), 0);
        glEnd();
    }

    for (vector<Option *>::iterator i = options.begin(); i != options.end(); i++) {
        hPos += 25;
        drawText(Vec3f(wres / 4, hPos, 0), (*i)->name, GLUT_BITMAP_HELVETICA_18);
    }
}

void SelectorMenu::highlightNext()
{
    highlighted++;
    if (highlighted >= options.size())
        highlighted = 0;
}

void SelectorMenu::highlightPrev()
{
    highlighted--;
    if (highlighted < 0)
        highlighted = (options.size() - 1);
}

void SelectorMenu::select()
{
    selected = highlighted;
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
    ss << "fps: " << Graphics::getInstance().fps();
    glColor3f(0,0,1);
    drawText(Vec3f(75, world.camera.getHres() - 10, 0), ss.str(),
             GLUT_BITMAP_HELVETICA_18);
}

FPS::FPS(Vec3f pos) : Widget(pos)
{}

void FPS::draw()
{
    Graphics &graphics = Graphics::getInstance();
    Physics &physics = Physics::getInstance();
    AIManager &aim = AIManager::getInstance();
    stringstream ss;
    glColor3f(0,1,0);
    ss << "gfx: " << graphics.frequency << " (" << graphics.fps() << ")";
    drawText(Vec3f(0,20,0), ss.str(), GLUT_BITMAP_HELVETICA_10);
    ss.seekp(0);
    ss << "ai : " << aim.frequency << " (" << aim.fps() << ")";
    drawText(Vec3f(0,30,0), ss.str(), GLUT_BITMAP_HELVETICA_10);
    ss.seekp(0);
    ss << "phs: " << physics.frequency << " (" << physics.fps() << ")";
    drawText(Vec3f(0,40,0), ss.str(), GLUT_BITMAP_HELVETICA_10);


}

WeaponDisplay::WeaponDisplay(Vec3f pos, Agent *agent) : 
    Widget(pos), agent(agent), world(World::getInstance())
{}

void WeaponDisplay::draw()
{
    stringstream ss;
    glColor3f(0, 1, 0);
    drawText(Vec3f(0, 10, 0), ss.str(), GLUT_BITMAP_HELVETICA_10);
    ss.seekp(0);
    switch(agent->steerInfo.weapon) {
    case NONE:
        {
            ss << "Weapon: None";
            break;
        }
    case SMACK: 
        {
            ss << "Weapon: Smack (segfault all)";
            break;
        }
    case RAYGUN: 
        {
            ss << "Weapon: Raygun (segfault line of sight)";
            break;
        }
    case BOXBOMB:
        {
            ss << "Weapon: Boxbomb";
            break;
        }
    default: break;
    }
    drawText(Vec3f(0, 10, 0), ss.str(), GLUT_BITMAP_HELVETICA_10);
    ss.seekp(0);
}
/*
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
*/
