#ifndef INPUT_H
#define INPUT_H

#include "Agents/player.h"
class Error;
class Client;

class Input
{
    static Input _instance;
    Input();
    ~Input();
    Input(const Input&);
    Input &operator=(const Input&);
    PlayerController *player;
    Client *client;
    Error *error;
  public:
    static Input &getInstance();
    const PlayerController &getPlayerController() const;
    int processInput();
    void controlPlayer(PlayerController *p);
};

#endif
