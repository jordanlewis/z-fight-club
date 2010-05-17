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

Scheduler::Scheduler() :
    world(&World::getInstance()),
    graphics(&Graphics::getInstance()),
    sound(&Sound::getInstance()),
    physics(&Physics::getInstance()),
    ai(&AIManager::getInstance()),
    input(&Input::getInstance()),
    client(&Client::getInstance()),
    server(&Server::getInstance()),
    error(&Error::getInstance()),
    profilerclock(0)
{
}

void Scheduler::soloLoopForever()
{
    int done = 0;
    int curCount = 0;
    double now;
    double last = GetTime();
    double sinceStart;

    timeStarted = last;
    raceState = COUNTDOWN;

    cout << "Looping forever...(solo)" << endl;
    while (!done)
    {
        done = input->processInput();

        now = GetTime();
        sinceStart = now - timeStarted;

        if (raceState <= COUNTDOWN)
        {
            if (sinceStart > curCount)
            {
                if (curCount == 3)
                {
                    cout << "GO!!!!!!!!" << endl;
                    raceState = RACE;
                }
                else
                    cout << 3 - curCount++ << "..." << endl;
            }
        }

        if (now - last > 0)
        {
            physics->simulate(now - last);
        }
        last = now;

        ai->run();
        graphics->render();
        sound->render();
        if ((profilerclock++ & 0x0F) == 0) error->pdisplay();

        usleep(10000);
    }
}

void Scheduler::clientLoopForever(){
    // draw_welcome_screen
    cout << "z fight club presents: Tensor Rundown" << endl << endl
         << "    up and down arrow keys accelerate forwards and backwards" << endl
         << "       left and right rotate your vehicle" << endl;

    while (1)
    {
        client->checkForAck();
        if (client->clientState == C_HAVEID)
        {
            cout << "aha, I must have heard from the server. I can send a start request now." << endl;
            cout << "  mash the spacebar to begin" << endl;
            break;
        }
    }
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
        client->checkForStart();
        if (client->clientState == C_START)
        {
            cout << "aha, I must have heard from the server. starting..." << endl;
            raceState = RACE;
            break;
        }
        if (GetTime() > t+5)
        {
            cout << "if I didn't get a start message by now, start anyway" << endl;
            raceState = RACE;
            break;
        }
    }

    int done = 0;
    double now;
    double last = GetTime();

    cout << "Looping forever...(client)" << endl;
    while (!done)
    {
        done = input->processInput();
        client->pushToServer();

        client->updateFromServer();
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

        if ((profilerclock++ & 0x0F) == 0) error->pdisplay();
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
        if ((profilerclock++ & 0x0F) == 0) error->pdisplay();
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

