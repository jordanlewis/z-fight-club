#include <SDL/SDL.h>
#include "input.h"
#include "Agents/player.h"

Input Input::_instance;

int Input::processInput()
{
    SDL_Event SDLevt;

    /* XXX little hack so that we can run without a player - jdoliner Apr 26 */
    while (SDL_PollEvent(&SDLevt) && player) {
        switch(SDLevt.type) {
            case SDL_KEYDOWN:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_q:
                        return 1;
                    case SDLK_LEFT:
                        player->setTurnState(LEFT); break;
                    case SDLK_RIGHT:
                        player->setTurnState(RIGHT); break;
                    case SDLK_UP:
                        player->setEngineState(ACCELERATE); break;
                    case SDLK_DOWN:
                        player->setEngineState(REVERSE); break;
                    default: break;
                } break;
            case SDL_KEYUP:
                switch (SDLevt.key.keysym.sym) {
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player->setTurnState(STRAIGHT); break;
                    case SDLK_UP:
                    case SDLK_DOWN:
                        player->setEngineState(NEUTRAL); break;
                    default: break;
                } break;
            case SDL_QUIT:
                return 1;
            default:
                break;
        }
    }
    if (player)
	player->updateAgent();
    return 0;
}

void Input::controlPlayer(PlayerController &p)
{
    player = &p;
}

Input &Input::getInstance()
{
    return _instance;
}

Input::Input()
{
}

Input::~Input()
{
}
