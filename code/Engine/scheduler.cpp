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

void Scheduler::welcomeScreen()
{
    cout << "z fight club presents: Tensor Rundown" << endl << endl
         << "    up and down arrow keys accelerate forwards and backwards" << endl
         << "       left and right rotate your vehicle" << endl;
}

void Scheduler::clientLoopForever()
{
    error->log(ENGINE, TRIVIAL, "Entering client loop\n");
    double now;
    double last = GetTime();
    client->clientState = C_CONNECTING;
    while (1)
    {
        switch (client->clientState)
        {
          case C_CONNECTING:
            client->checkForPackets(); // may transition us into C_CONNECTED
            break;
          case C_CONNECTED:
            cout << endl << "mash the spacebar to begin" << endl;
            client->clientState = C_WAITINGFORPLAYER;
            break;
          case C_WAITINGFORPLAYER:
            input->processInput(); // may transition us into C_PLAYERREADYTOSTART
            break;
          case C_PLAYERREADYTOSTART:
            client->sendStartRequest();
            client->clientState = C_WAITINGFORSTART;
            break;
          case C_WAITINGFORSTART:
            client->checkForPackets(); // may transition us into C_RACE
            raceState = RACE;
            break;
          case C_RACE:
            client->checkForPackets();
            client->pushToServer();
            now = GetTime();
            if (now - last > 0) physics->simulate(now - last);
            last = now;
            graphics->render();
            sound->render();
            input->processInput(); // may transition us into C_DONE
            if ((profilerclock++ & 0x0F) == 0) error->pdisplay();
            break;
          case C_DONE:
            client->disconnect();
            return;
          default:
            assert(0);
            break;
        }
    }
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
        graphics->render();
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

