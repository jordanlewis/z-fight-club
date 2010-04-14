#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "world.h"

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
  public:
    Scheduler();
    void schedule(ComponentEvent &evt);
    void loopForever(World *world);
    void InitGraphics();
};

#endif
