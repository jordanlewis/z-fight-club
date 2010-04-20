#include <queue>
#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"
#include "world.h"
#include "Agents/input.h"
#include "Utilities/defs.h"

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
    physics(&Physics::getInstance())
{
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

    Input &input = Input::getInstance();

    cout << "Looping forever..." << endl;
    while (!done)
    {
        /* Grab input from SDL loop */
        SDL_Event SDLevt;
        while (SDL_PollEvent(&SDLevt)) {
            switch(SDLevt.type) {
                case SDL_KEYDOWN:
                    switch (SDLevt.key.keysym.sym) {
                        case SDLK_LEFT:
                            input.setTurnState(LEFT); break;
                        case SDLK_RIGHT:
                            input.setTurnState(RIGHT); break;
                        case SDLK_UP:
                            input.setEngineState(ACCELERATE); break;
                        case SDLK_DOWN:
                            input.setEngineState(REVERSE); break;
                        default: break;
                    } break;
                case SDL_KEYUP:
                    switch (SDLevt.key.keysym.sym) {
                        case SDLK_LEFT:
                        case SDLK_RIGHT:
                            input.setTurnState(STRAIGHT); break;
                        case SDLK_UP:
                        case SDLK_DOWN:
                            input.setEngineState(NEUTRAL); break;
                        default: break;
                    } break;
                case SDL_QUIT:
                    done = 1;
                    break;
                default:
                    break;
            }
        }

        input.updateAgent();

        now = GetTime();
        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;

        graphics->render();
        sound->process_queue();

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
}

Scheduler::~Scheduler()
{
}

Scheduler &Scheduler::getInstance()
{
    return _instance;
}

