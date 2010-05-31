#ifndef HUD_H
#define HUD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include "Engine/scheduler.h"
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
        virtual void highlightNext() {};
        virtual void highlightPrev() {};
        virtual bool select() {return true;};
        virtual bool up() {return true;};
        virtual void inputChar(char) {};
        virtual void backspace() {};
};

class SubMenu : public Menu
{
    public:
        SubMenu         *parent;        /* !< the menu above this one, NULL if top level */
        int             highlighted;    /* !< which item is highlighted */
        int             selected;       /* !< which submenu we've selectED -1 if we're still in this level */
        RaceState_t     unpause;        /* !< what to set the scheduler to when we unpause */
        vector<Menu *>    items;        /* !< the items in the menu */
        SubMenu(string);
        SubMenu(string, vector<Menu *>);
        void draw();
        void highlightNext();          /* !< advance the selection */
        void highlightPrev();          /* !< unadvance the selection */
        bool select();                 /* !< make the highlighted item the selected */
        bool up();                     /* !< up one menu level */
        void reset();                  /* !< reset selected and highlighted items */
        void inputChar(char);          /* !< pass a char in (for text boxs) */
        void backspace();              /* !< pop a character off a textbox */
};

class TerminalMenu : public Menu
{
    public:
        void (*callback)();     /* !< function to call when this button is hit */
        TerminalMenu(string, void (*callback)());
        void draw();
        void highlightNext() {};          /* !< advance the selection */
        void highlightPrev() {};          /* !< unadvance the selection */
        bool select();
        bool up() {return true;};
        void inputChar(char);
        void backspace();
};

class TextboxMenu : public Menu
{
    public:
        SubMenu     *parent;        /* !< the menu above this one */
        string      entered;        /* !< what's been entered into the text box */
        TextboxMenu(string);
        void draw();
        void highlightNext() {};          /* !< advance the selection */
        void highlightPrev() {};          /* !< unadvance the selection */
        bool select();
        bool up() {return true;};
        void reset();
        void inputChar(char);        /* !< pass a char in */
        void backspace();            /* !< take a char out */
};

class Option
{
    public:
        string      name;           /* !< name of the selection */
        GLuint      texid;          /* !< image for the selection */
        Option(string name, GLuint texid) :name(name), texid(texid) {}; 
};

class SelectorMenu : public Menu
{
    public:
        SubMenu             *parent;        /* !< the menu above this one */
        int                 highlighted;    /* !< which selection is highlighted */
        int                 selected;       /* !< which selection has actually been selected */
        vector<Option *>    options;        /* !< things that can be selected */
        SelectorMenu(string, vector<Option *>);
        void draw();
        void highlightNext();          /* !< advance the selection */
        void highlightPrev();          /* !< unadvance the selection */
        bool select();
        bool up() {return true;};
        void reset() {};
        void inputChar(char) {};
        void backspace() {};
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

class FPS : public Widget
{
  public:
    FPS(Vec3f);
    void draw();
};

class WeaponDisplay : public Widget
{
 public:
    Agent *agent;
    World &world;
    WeaponDisplay(Vec3f pos, Agent *agent);
    void draw();
};

#endif
