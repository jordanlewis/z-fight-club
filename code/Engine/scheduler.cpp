#include <queue>
#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"
#include "world.h"
#include "input.h"
#include "Utilities/defs.h"
#include "Agents/ai.h"

using namespace std;

Scheduler Scheduler::_instance;

ComponentEvent::ComponentEvent(double when, Component_t which)
{
    at = when;
    component = which;
}

bool ComponentEvent::operator< (const ComponentEvent &evt) const
{
    return evt.at < at;
}

Scheduler::Scheduler() :
    world(&World::getInstance()),
    graphics(&Graphics::getInstance()),
    sound(&Sound::getInstance()),
    physics(&Physics::getInstance()),
    ai(&AIManager::getInstance())
{
}

void Scheduler::schedule(ComponentEvent &evt)
{
    eventQueue.push(evt);
}

void Scheduler::soloLoopForever()
{
    int done = 0;
    double now;
    double last = GetTime();

    Input &input = Input::getInstance();
    AIManager &ai = AIManager::getInstance();

    cout << "Looping forever..." << endl;
    while (!done)
    {
        /* Grab input from SDL loop */
        done = input.processInput();

        now = GetTime();
        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;

        ai.run();
        graphics->render();
        sound->render();
	ai.run();

        usleep(10000);


#ifdef USING_COMPLICATED_SCHEDULER
        if (eventQueue.empty())
            continue;

        const ComponentEvent &evt = eventQueue.top();
        if (evt.at < now)
        {
            switch (evt.component)
            {
                case AI:
                case GRAPHICS:
                case INPUT:
                case NETWORK:
                case PHYSICS:
                case SOUND:
                default: break;
            }
        }
#endif

    }

    /* clean everything up */
    /* SDL_CloseAudio(); */
}

Scheduler::~Scheduler()
{
}

Scheduler &Scheduler::getInstance()
{
    return _instance;
}

