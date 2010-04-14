#include "scheduler.h"
#include "world.h"
#include "SDL/SDL.h"
#include <queue>
extern "C" {
    #include "defs.h"
}


ComponentEvent::ComponentEvent(double when, Component_t which)
{
    at = when;
    component = which;
}

bool ComponentEvent::operator< (const ComponentEvent &evt) const
{
    return evt.at < at;
}

Scheduler::Scheduler()
{
    eventQueue = std::priority_queue<ComponentEvent>();
    graphics.InitGraphics();
}

void Scheduler::schedule(ComponentEvent &evt)
{
    eventQueue.push(evt);
}

void Scheduler::loopForever(World *world)
{
    int done = 0;
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

        double now = GetTime();

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
    }
}
