#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "world.h"
#include "Graphics/graphics.h"
#include "Physics/physics.h"
#include "Sound/sound.h"
#include "Agents/ai.h"

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

    World    *world;
    Graphics *graphics;
    Sound    *sound;
    Physics  *physics;
    AIManager *ai;

    static Scheduler _instance;
    Scheduler();
    ~Scheduler();
    Scheduler(const Scheduler&);
    Scheduler &operator=(const Scheduler&);
  public:
    void schedule(ComponentEvent &evt);
    void soloLoopForever();
    void clientLoopForever();
    void serverLoopForever();

    static Scheduler &getInstance();
};

#endif
