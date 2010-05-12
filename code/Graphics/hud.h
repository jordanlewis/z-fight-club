#ifndef HUD_H
#define HUD_H

#include "Utilities/vec3f.h"
#include "Agents/agent.h"

class Widget
{
  public:
      Vec3f	pos;	/* !< ofcourse we'll only be using x and y */
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
#endif
