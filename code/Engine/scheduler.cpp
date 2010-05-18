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

    error->log(ENGINE, TRIVIAL, "Entering solo-play loop\n");
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

void Scheduler::clientLoopForever()
{
    cout << "z fight club presents: Tensor Rundown" << endl << endl
         << "    up and down arrow keys accelerate forwards and backwards" << endl
         << "       left and right rotate your vehicle" << endl;

    while (1)
    {
        client->checkForAck();
        if (client->clientState == C_HAVEID)
        {
            cout << endl << "mash the spacebar to begin" << endl;
            break;
        }
    }
    SDL_Event SDLevt;
    while (1)
    {
        usleep(1000);
        if (SDL_PollEvent(&SDLevt))
        {
            if ((SDLevt.type == SDL_KEYDOWN) &&
                (SDLevt.key.keysym.sym == SDLK_SPACE))
            {
                client->sendStartRequest();
                break;
            }
            else if (SDLevt.type == SDL_QUIT)
            {
                    return;
            }
        }
    }
    while (1)
    {
        client->checkForStart();
        if (client->clientState == C_START)
        {
            raceState = RACE;
            break;
        }
    }

    int done = 0;
    double now;
    double last = GetTime();

    error->log(ENGINE, TRIVIAL, "Entering client loop\n");
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

        graphics->render();
        sound->render();

        if ((profilerclock++ & 0x0F) == 0) error->pdisplay();
        usleep(10000);
    }
    client->disconnect();
    return;
}

void Scheduler::serverLoopForever()
{
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

