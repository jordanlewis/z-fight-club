#ifndef HUD_H
#define HUD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include <list>
extern "C" {
        #include "Utilities/load-png.h"
}
#include <vector>
using namespace std;

class Widget
{
  public:
      Vec3f    pos;     /* !< ofcourse we'll only be using x and y */
      virtual void draw() {return;}
      Widget(Vec3f);
};

class Speedometer : public Widget
{
  public:
    Agent *agent;
    Image2D_t *background;
    Speedometer(Vec3f, Agent *);
    void draw();
};

class LapCounter : public Widget
{
  public:
    Agent *agent;
    LapCounter(Vec3f, Agent *);
    void draw();
};

class StopLight : public Widget
{
  public:
    StopLight(Vec3f);
    int nLit;
    void draw();
};

class Text : public Widget
{
  public:
    Text(Vec3f, string text);
    string text;
    void draw();
};

class Menu : public Widget
{
    public:
        string name;
        Menu(Vec3f, string);
        virtual void draw();
};

class SubMenu : public Menu
{
    public:
        SubMenu         *up;        /* !< the menu above this one */
        int             selection;  /* !< which item is selected */
        list<Menu *>    items;      /* !< the items in the menu */
        SubMenu(Vec3f, string);
        SubMenu(Vec3f, string, list<Menu *>);
        void draw();
};

class TerminalMenu : public Menu
{
    public:
        void (*callback)();     /* !< function to call when this button is hit */
        TerminalMenu(Vec3f, string, void (*callback)());
};

class MiniMap : public Widget
{
  public:
    MiniMap(Vec3f, Path *path);
    Path *path;
    void draw();
};

class Places : public Widget
{
  public:
    Places(Vec3f);
    void draw();
};

#endif
