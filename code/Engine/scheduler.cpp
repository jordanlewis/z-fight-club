#include <queue>
#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"
#include "world.h"
#include "Utilities/defs.h"

using namespace std;

ComponentEvent::ComponentEvent(double when, Component_t which)
{
    at = when;
    component = which;
}

bool ComponentEvent::operator< (const ComponentEvent &evt) const
{
    return evt.at < at;
}

Scheduler::Scheduler(World *world, Graphics *graphics, Physics *physics)
{
    this->world = world;
    this->graphics = graphics;
    this->physics = physics;
}

void Scheduler::schedule(ComponentEvent &evt)
{
    eventQueue.push(evt);
}

void Scheduler::loopForever()
{
    int done = 0;
    double now;
    double last = GetTime();

    cout << "Looping forever..." << endl;
    while (!done)
    {
	/* Grab input from SDL loop */
	SDL_Event SDLevt;
	while (SDL_PollEvent(&SDLevt)) {
	    switch(SDLevt.type) {
		case SDL_QUIT:
		    done = 1;
		    break;
	    }
	}



        now = GetTime();
        /* if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now; */

        graphics->render(world);


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
}
