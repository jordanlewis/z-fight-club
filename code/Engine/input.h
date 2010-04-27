#ifndef INPUT_H
#define INPUT_H

#include "Agents/player.h"

class Input
{
    static Input _instance;
    Input();
    ~Input();
    Input(const Input&);
    Input &operator=(const Input&);
    PlayerController *player;
  public:
    static Input &getInstance();
    const PlayerController &getPlayerController() const;
    int processInput();
    void controlPlayer(PlayerController &p);
};

#endif
