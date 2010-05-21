#include <SDL/SDL.h>
#include "input.h"
#include "world.h"
#include "Network/client.h"
#include "Agents/player.h"
#include "Utilities/error.h"

Input Input::_instance;

int Input::processInput()
{
    World &world = World::getInstance();

    error->pin(P_INPUT);
    SDL_Event SDLevt;
    Uint8 *keystate = SDL_GetKeyState(NULL);

    /* XXX little hack so that we can run without a player - jdoliner Apr 26 */
    while (SDL_PollEvent(&SDLevt) && player) {
        switch(SDLevt.type) {
            case SDL_KEYDOWN:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_q:
                        error->pout(P_INPUT);
                        return 1;
                    case SDLK_LEFT:
                        player->setTurnState(LEFT); break;
                    case SDLK_RIGHT:
                        player->setTurnState(RIGHT); break;
                    case SDLK_UP:
                        player->setEngineState(ACCELERATE); break;
                    case SDLK_DOWN:
                        player->setEngineState(REVERSE); break;
                    case SDLK_TAB:
                        player->setWeaponState(CHANGE); break;
                    case SDLK_f:
                        player->setWeaponState(FIRE); break;
                    case SDLK_c:
                        World::getInstance().camera.cycleView(); break;
                    case SDLK_RETURN:
                        if (client->clientState == C_WAITINGFORPLAYER)
                            client->clientState = C_PLAYERHASJOINED;
                        break;
                    case SDLK_SPACE:
                        if (client->clientState == C_WAITINGFORPLAYER ||
                            client->clientState == C_PLAYERHASJOINED)
                            client->clientState = C_PLAYERREADYTOSTART;
                        break;
                    default: break;
                } break;
            case SDL_KEYUP:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (keystate[SDLK_RIGHT]) {
                            player->setTurnState(RIGHT);
                            break;
                        }
                    case SDLK_RIGHT:
                        if (keystate[SDLK_LEFT]) {
                            player->setTurnState(LEFT);
                            break;
                        }
                        player->setTurnState(STRAIGHT);
                        break;
                    case SDLK_UP:
                        if (keystate[SDLK_DOWN]) {
                            player->setEngineState(REVERSE);
                            break;
                        }
                    case SDLK_DOWN:
                        if (keystate[SDLK_UP]) {
                            player->setEngineState(ACCELERATE);
                            break;
                        }
                        player->setEngineState(NEUTRAL);
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
    error(&Error::getInstance())
{
    player = new PlayerController();
}

Input::~Input()
{
}
