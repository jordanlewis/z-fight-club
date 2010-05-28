#include <SDL/SDL.h>
#include "input.h"
#include "world.h"
#include "Network/client.h"
#include "Agents/player.h"
#include "Utilities/error.h"
#include "Engine/scheduler.h"

Input Input::_instance;

int Input::processInput()
{
    World &world = World::getInstance();

    error->pin(P_INPUT);
    SDL_Event SDLevt;
    Uint8 *keystate = SDL_GetKeyState(NULL);

    /* XXX different hack so that we can run get input without a player - nick May 24 */
    while (SDL_PollEvent(&SDLevt)) {
        switch(SDLevt.type) {
            case SDL_KEYDOWN:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_q:
                        if (world.runType == CLIENT) client->clientState = C_DONE;
                        if (world.runType == SERVER) scheduler->raceState = ALL_DONE;
                        error->pout(P_INPUT);
                        return 1;
                    case SDLK_LEFT:
                        if (player) player->setTurnState(LEFT); break;
                    case SDLK_RIGHT:
                        if (player) player->setTurnState(RIGHT); break;
                    case SDLK_UP:
                        if (Scheduler::getInstance().raceState == RACE)
                            if (player) 
                                player->setEngineState(ACCELERATE); 
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world.pauseMenu->highlightPrev();
                        break;
                    case SDLK_DOWN:
                        if (Scheduler::getInstance().raceState == RACE)
                            if (player) 
                                player->setEngineState(REVERSE); 
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world.pauseMenu->highlightNext();
                        break;
                    case SDLK_TAB:
                        if (player) player->setWeaponState(CHANGE); break;
                    case SDLK_f:
                        if (player) player->setWeaponState(FIRE); break;
                    case SDLK_c:
                        World::getInstance().camera.cycleView(); break;
                    case SDLK_p:
                        if (Scheduler::getInstance().raceState == PAUSE) {
                            world.pauseMenu->reset();
                            Scheduler::getInstance().raceState = RACE;
                        } else
                            Scheduler::getInstance().raceState = PAUSE;
                        break;
                    case SDLK_RETURN:
                        if (Scheduler::getInstance().raceState == PAUSE)
                            world.pauseMenu->select();
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
                        if (Scheduler::getInstance().raceState == PAUSE) {
                            world.pauseMenu->up();
                        }
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
                world.camera.wres = SDLevt.resize.w;
                world.camera.hres = SDLevt.resize.h;

                SDL_SetVideoMode(world.camera.wres, world.camera.hres, 32, SDL_OPENGL|SDL_RESIZABLE);
                break;
            case SDL_QUIT:
                error->pout(P_INPUT);
                client->clientState = C_DONE;
                return 1;
            default:
                break;
        }
    }
    if (player)
        player->updateAgent();
    error->pout(P_INPUT);
    return 0;
}

PlayerController &Input::getPlayerController()
{
    return *player;
}

void Input::controlPlayer(PlayerController *p)
{
    player = p;
}

Input &Input::getInstance()
{
    return _instance;
}

Input::Input() :
    client(&Client::getInstance()),
    error(&Error::getInstance()),
    scheduler(&Scheduler::getInstance())
{
    player = new PlayerController();
}

Input::~Input()
{
}
