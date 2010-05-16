#ifndef HUD_H
#define HUD_H

#include "Utilities/vec3f.h"
#include "Agents/agent.h"
#include <vector>

class Widget
{
  public:
      Vec3f    pos;     /* !< ofcourse we'll only be using x and y */
      virtual void draw() {return;}
};

class Speedometer : public Widget
{
  public:
    Agent *agent;
    Speedometer(Vec3f, Agent *);
    ~Speedometer();
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
