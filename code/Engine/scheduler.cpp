#include <queue>
#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"
#include "world.h"
#include "input.h"
#include "Utilities/defs.h"
#include "Agents/ai.h"
#include "Network/server.h"
#include "Network/client.h"
#include "Graphics/graphics.h"
#include "Graphics/hud.h"
#include "Physics/physics.h"
#include "Sound/sound.h"
#include "Sound/sobject.h"
#include "Utilities/error.h"


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
    error(&Error::getInstance())
{
}

void Scheduler::setupLoopForever()
{
    double now;
    double last = GetTime();
    double sinceStart;
    raceState = SETUP;

    while (raceState == SETUP)
    {
        now = GetTime();
        sinceStart = now - timeStarted;
        last = now;
        graphics->render();
        input->processInput();
    }
}

void Scheduler::soloLoopForever()
{
    double nextLightTime = 0;
    double now;
    double last = GetTime();
    double sinceStart;
    bool killLight = false;
    //raceState = SETUP;

    error->log(ENGINE, TRIVIAL, "Entering solo-play loop\n");
    StopLight *sl = NULL;
    while (raceState != ALL_DONE)
    {
        input->processInput();
        now = GetTime();
        sinceStart = now - timeStarted;

        if (raceState == COUNTDOWN)
        {
            if (sl == NULL)
            {
                sl = new StopLight(Vec3f(0,0,0));
                world->widgets.push_back(sl);
            }
            if (sinceStart > nextLightTime)
            {
                string s;
                if (nextLightTime < 1.4)
                {
                    s = "23670.wav";
                }
                else
                {
                    s = "23670L.wav";
                }
                CameraFollower *c = new CameraFollower(NULL,
                                                       NULL,
                                                       new SObject(s, GetTime(), AL_FALSE, 1.0),
                                                       NULL,
                                                       &world->camera);
                world->addObject(c);
                sl->nLit++;
                nextLightTime += 0.5;
                if (nextLightTime > 1.9)
                {
                    raceState = RACE;
                    killLight = true;
                }
            }
        }
        if (killLight && sinceStart > 1.6)
        {
            killLight = false;
            world->widgets.pop_back();
            delete sl;
        }

        switch (raceState) {

            case RACE:
            case COUNTDOWN:
                if (now - last > 0)
                {
                    physics->simulate(now - last);
                }
                last = now;

                ai->run();
                graphics->render();
                sound->render();
                break;
            case PAUSE:
                graphics->render();
                last = now;
                break;
            case SETUP:
                graphics->render();
                last = now;
                break;
            case PLAYER_DONE:
            case ALL_DONE:
                // Do nothing
                break;
        }
        world->cleanObjects();

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
    double nowPh, nowNet;
    double lastPh = GetTime();
    double lastNet = GetTime();
    client->clientState = C_CONNECTING;
    while (1)
    {
        switch (client->clientState)
        {
          case C_CONNECTING:
            client->checkForPackets(); // may transition us into C_CONNECTED
            break;
          case C_CONNECTED:
            cout << endl << "hit enter to join" << endl;
            cout << endl << "mash the spacebar to start the race" << endl;
            client->clientState = C_WAITINGFORPLAYER;
            break;
          case C_WAITINGFORPLAYER:
            input->processInput(); /* may transition into C_PLAYERREADYTOSTART
                                    * or C_PLAYERHASJOINED */
            client->checkForPackets();
            if (client->clientState == C_PLAYERHASJOINED) //transition inc
                client->sendJoinRequest();
            break;
          case C_PLAYERHASJOINED:
            input->processInput(); //may transition into C_PLAYERREADYTOSTART
            client->checkForPackets();
            break;
          case C_PLAYERREADYTOSTART:
            client->sendStartRequest();
            client->clientState = C_WAITINGFORSTART;
            break;
          case C_WAITINGFORSTART:
            client->checkForPackets(); // may transition us into C_RACE
            break;
          case C_RACE:
            client->checkForPackets();
            client->updateAgentsLocally();
            client->updateDummyController();
            nowNet = GetTime();
            if (nowNet - lastNet > SC_CLIENT_UPDATE_FREQ_SECONDS) 
                {
                    client->pushToServer();
                    lastNet = nowNet;
                }        
            nowPh = GetTime();
            if (nowPh - lastPh > 0) physics->simulate(nowPh - lastPh);
            lastPh = nowPh;
            ai->run();
            graphics->render();
            sound->render();
            input->processInput(); // may transition us into C_DONE
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
    server->createAllAIAgents();
    server->gatherPlayers();
    raceState = RACE;
    server->createAll();
    double nowPh, nowNet;
    double lastPh = GetTime();
    double lastNet = GetTime();
    while (raceState != ALL_DONE)
    {
        input->processInput(); /* hoping to get quit */
        nowPh = GetTime();
        if (nowPh - lastPh > 0)
        {
            physics->simulate(nowPh - lastPh);
        }
        lastPh = nowPh;
        ai->run();
        graphics->render();
        nowNet = GetTime();
        if (nowNet - lastNet > SC_SERVER_UPDATE_FREQ_SECONDS)
            {
                server->pushAgents();
                lastNet = nowNet;
            } 
        server->serverFrame();
        server->updateAgentsLocally();
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

