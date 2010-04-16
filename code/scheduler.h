#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "world.h"
#include "graphics.h"
#include "physics.h"

typedef enum
{
    AI,
    GRAPHICS,
    INPUT,
    NETWORK,
    PHYSICS,
    SOUND
} Component_t;

class ComponentEvent
{
  public:
    double at;             /* !<when to run component */
    Component_t component; /* !<what component to run */
    bool operator< (const ComponentEvent &evt) const;
    ComponentEvent(double when, Component_t which);
};

class Scheduler
{
    std::priority_queue <ComponentEvent> eventQueue; /* !<queue of events */

    Graphics *graphics;
    World *world;
    Physics *physics;
  public:
    Scheduler(World *world, Graphics *graphics, Physics *physics);
    void schedule(ComponentEvent &evt);
    void loopForever();
};

#endif
