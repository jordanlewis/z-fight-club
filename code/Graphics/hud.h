#ifndef HUD_H
#define HUD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include <list>
#include <vector>
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

class Menu
{
    public:
        string name;
        Menu(string);
        virtual void draw() {};
        virtual void select() {};
};

class SubMenu : public Menu
{
    public:
        SubMenu         *parent;    /* !< the menu above this one, NULL if top level */
        int             highlighted;  /* !< which item is highlighted */
        int             selected;    /* !< which submenu we've selectED -1 if we're still in this level */
        vector<Menu *>    items;      /* !< the items in the menu */
        SubMenu(string);
        SubMenu(string, vector<Menu *>);
        void draw();
        void highlightNext();          /* !< advance the selection */
        void highlightPrev();          /* !< unadvance the selection */
        void select();                 /* !< make the highlighted item the selected */
        bool up();                     /* !< up one menu level */
        void reset();                  /* !< reset selected and highlighted items */
};

class TerminalMenu : public Menu
{
    public:
        void (*callback)();     /* !< function to call when this button is hit */
        TerminalMenu(string, void (*callback)());
        void select();
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
