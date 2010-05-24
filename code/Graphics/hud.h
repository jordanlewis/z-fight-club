#ifndef HUD_H
#define HUD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
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
