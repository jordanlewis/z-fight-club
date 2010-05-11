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
    ai(&AIManager::getInstance()),
    input(&Input::getInstance()),
    client(&Client::getInstance()),
    server(&Server::getInstance())

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

    cout << "Looping forever...(solo)" << endl;
    while (!done)
    {
        /* Grab input from SDL loop */
        done = input->processInput();

        now = GetTime();
        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;

        ai->run();
        graphics->render();
        sound->render();
        ai->run();

        usleep(10000);
    }

    /* clean everything up */
    /* SDL_CloseAudio(); */
}

void Scheduler::clientLoopForever(){
    // draw_welcome_screen
    cout << "z fight club presents: Tensor Rundown" << endl << endl
         << "    up and down arrow keys accelerate forwards and backwards" << endl
         << "       left and right rotate your vehicle" << endl
         << "  mash the spacebar to begin" << endl;
    cout.flush();

    // wait for space, network "go", quit, or disconnect
    // maybe processInput needs siblings appropriate for various states
    double t = GetTime();
    SDL_Event SDLevt;
    while (1)
    {
        if (SDL_PollEvent(&SDLevt))
        {
            switch(SDLevt.type)
            {
                case SDL_KEYDOWN:
                    if (SDLevt.key.keysym.sym == SDLK_SPACE)
                    {
                        cout << "asking the server to start the game..." << endl;
                        client->sendStartRequest();
                        t = GetTime();
                    }
                    break;
                case SDL_QUIT:
                    cout << "telling the server nevermind" << endl;
                    return;
            }
        }
        if (GetTime() > t+5) // pretend there's a network "go" or disconnect message
        {
            cout << "ah, a message from the server" << endl
                 << "must be time to play" << endl;
            break;
        }
        usleep(10000);
    }

    int done = 0;
    double now;
    double last = GetTime();

    cout << "Looping forever...(client)" << endl;
    while (!done)
    {
        /* Grab input from SDL loop */
        done = input->processInput();
        client->pushToServer();

        // client->updateFromServer();
        // physics will operate on latest data from server, right?
        now = GetTime();
        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;
        // ai will be replaced by server info, right?
        ai->run();

        graphics->render();
        sound->render();

        usleep(10000);
    }
    return;
}

void Scheduler::serverLoopForever(){
    server->gatherPlayers();
    double now;
    double last = GetTime();
    while (1)
    {
        now = GetTime();
        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;
        ai->run();
        server->serverFrame();
    }
    return;
}

Scheduler::~Scheduler()
{
}

Scheduler &Scheduler::getInstance()
{
    return _instance;
}

