#include <SDL/SDL.h>
#include "input.h"
#include "world.h"
#include "Network/server.h"
#include "Network/client.h"
#include "Sound/sound.h"
#include "Agents/player.h"
#include "Utilities/error.h"
#include "Engine/scheduler.h"

Input Input::_instance;

/*! processInput
 *  \brief parse an SDL_Event from SDL and send it to the appropriate module.
 */
int Input::processInput()
{
    SDL_Event SDLevt;
    Uint8 *keystate = SDL_GetKeyState(NULL);

    /* XXX different hack so that we can run get input without a player - nick May 24 */
    while (SDL_PollEvent(&SDLevt)) {
        switch(SDLevt.type) {
            case SDL_KEYDOWN:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_q:
                        if (world->runType == CLIENT) client->clientState = C_DONE;
                        if ((world->runType == SERVER) || (world->runType == SOLO)) scheduler->raceState = ALL_DONE;
                    case SDLK_LEFT:
                        if (player) player->setTurnState(LEFT); break;
                    case SDLK_RIGHT:
                        if (player) player->setTurnState(RIGHT); break;
                    case SDLK_PAGEUP:
                        sound->setVol(sound->getVol()+0.1);
                        break;
                    case SDLK_PAGEDOWN:
                        sound->setVol(sound->getVol()-0.1);
                        break;
                    case SDLK_UP:
                        if (player) 
                            player->setEngineState(ACCELERATE); 
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->highlightPrev();
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->highlightPrev();
                        break;
                    case SDLK_DOWN:
                        if (player) 
                            player->setEngineState(REVERSE); 
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->highlightNext();
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->highlightNext();
                        break;
                    case SDLK_TAB:
                        if (player) player->setWeaponState(CHANGE); break;
                    case SDLK_f:
                        if (player) player->setWeaponState(FIRE); break;
                    case SDLK_c:
                        World::getInstance().camera.cycleView(); break;
                    case SDLK_p:
                        if (Scheduler::getInstance().raceState == PAUSE)
                        {
                            if (world->runType == CLIENT)
                            {
                                client->sendUnpause();
                                client->clientState = C_RACE;
                            }
                            else if (world->runType == SERVER) server->sendUnpause();
                            world->pauseMenu->reset();
                            Scheduler::getInstance().raceState = RACE;
                        }
                        else if (Scheduler::getInstance().raceState == RACE)
                        {
                            if (world->runType == CLIENT)
                            {
                                client->sendPause();
                                client->clientState = C_PAUSE;
                            }
                            else if (world->runType == SERVER) server->sendPause();
                            Scheduler::getInstance().raceState = PAUSE;
                        }
                        break;
                    case SDLK_RETURN:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->select();
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->select();
                        else {
                            if (client->clientState == C_WAITINGFORPLAYER)
                                client->clientState = C_PLAYERHASJOINED;
                        }
                        break;
                    case SDLK_SPACE:
                        if (client->clientState == C_WAITINGFORPLAYER ||
                            client->clientState == C_PLAYERHASJOINED)
                            client->clientState = C_PLAYERREADYTOSTART;
                        break;
                    case SDLK_ESCAPE:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->up();
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->up();
                        break;
                    case SDLK_0:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('0');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('0');
                        break;
                    case SDLK_1:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('1');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('1');
                        break;
                    case SDLK_2:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('2');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('2');
                        break;
                    case SDLK_3:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('3');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('3');
                        break;
                    case SDLK_4:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('4');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('4');
                        break;
                    case SDLK_5:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('5');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('5');
                        break;
                    case SDLK_6:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('6');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('6');
                        break;
                    case SDLK_7:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('7');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('7');
                        break;
                    case SDLK_8:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('8');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('8');
                        break;
                    case SDLK_9:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('9');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('9');
                        break;
                    case SDLK_PERIOD:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->inputChar('.');
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->inputChar('.');
                        break;
                    case SDLK_BACKSPACE:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world->pauseMenu->backspace();
                        if (Scheduler::getInstance().raceState == SETUP)
                            world->setupMenu->backspace();
                        break;
                    default: break;
                } break;
            case SDL_KEYUP:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (keystate[SDLK_RIGHT]) {
                            if (player) player->setTurnState(RIGHT);
                            break;
                        }
                    case SDLK_RIGHT:
                        if (keystate[SDLK_LEFT]) {
                            if (player) player->setTurnState(LEFT);
                            break;
                        }
                        if (player) player->setTurnState(STRAIGHT);
                        break;
                    case SDLK_UP:
                        if (keystate[SDLK_DOWN]) {
                            if (player) player->setEngineState(REVERSE);
                            break;
                        }
                    case SDLK_DOWN:
                        if (keystate[SDLK_UP]) {
                            if (player) player->setEngineState(ACCELERATE);
                            break;
                        }
                        if (player) player->setEngineState(NEUTRAL);
                        break;
                    default: break;
                } break;
            case SDL_VIDEORESIZE:
                world->camera.wres = SDLevt.resize.w;
                world->camera.hres = SDLevt.resize.h;

                SDL_SetVideoMode(world->camera.wres, world->camera.hres, 32, SDL_OPENGL|SDL_RESIZABLE);
                break;
            case SDL_QUIT:
                if (world->runType == CLIENT) client->clientState = C_DONE;
                if ((world->runType == SERVER) || (world->runType == SOLO)) scheduler->raceState = ALL_DONE;
            default:
                break;
        }
    }
    if (player)
        player->updateAgent();
    return 0;
}

/*! getPlayerController
 *  \brief return the player which is currently being controlled by the controller
 */
PlayerController &Input::getPlayerController()
{
    return *player;
}

/*! controlPlayer
 *  \brief link to a PlayerController passing the input to it
 *  \param p the player controller to link with
 */
void Input::controlPlayer(PlayerController *p)
{
    player = p;
}

/*! releasePlayer
 *  \brief break the link with a player controller (also delete the controller)
 */
void Input::releasePlayer()
{
    delete player;
    player = new PlayerController();
}

/*! getInstance
 *  \brief return the single instantation of Input
 */
Input &Input::getInstance()
{
    return _instance;
}

/*! Input
 *  \brief default contructor, allocates a new PlayerController
 */
Input::Input() :
    client(&Client::getInstance()),
    world(&World::getInstance()),
    sound(&Sound::getInstance()),
    server(&Server::getInstance()),
    error(&Error::getInstance()),
    scheduler(&Scheduler::getInstance())
{
    player = new PlayerController();
}

/* !~Input
 * \brief Input destructor
Input::~Input()
{
}
