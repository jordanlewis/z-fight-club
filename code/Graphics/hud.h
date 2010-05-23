#ifndef HUD_H
#define HUD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
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

/* \brief Hud, heads up display class, this includes 2d graphics
 * that aren't actually huds like menus displays
 */
class Hud
{
  public:
    vector<Widget>    widget;     /* !< array of widgets */
    Hud();
    ~Hud();
    static Hud _instance;
    static Hud &getInstance();
};

#endif
