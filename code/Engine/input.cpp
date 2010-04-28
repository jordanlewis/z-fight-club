#include <SDL/SDL.h>
#include "input.h"
#include "Agents/player.h"

Input Input::_instance;

int Input::processInput()
{
    SDL_Event SDLevt;
    Uint8 *keystate = SDL_GetKeyState(NULL);

    while (SDL_PollEvent(&SDLevt)) {
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
		    case SDLK_f:
			player->setWeaponState(FIRE); break;
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
            case SDL_QUIT:
                return 1;
            default:
                break;
        }
    }
    player->updateAgent();
    return 0;
}

const PlayerController &Input::getPlayerController() const
{
    return *player;
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
    player = new PlayerController();
}

Input::~Input()
{
}
