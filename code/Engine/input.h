#ifndef INPUT_H
#define INPUT_H

#include "allclasses.h"

/* !Input
 * \brief Singleton class responsible for mainting the link between a users
 * input device and an in game racer
 */
class Input
{
    static Input _instance;
    Input();
    ~Input();
    Input(const Input&);
    Input &operator=(const Input&);
    PlayerController *player;
    Client *client;
    World *world;
    Sound *sound;
    Server *server;
    Error *error;
    Scheduler *scheduler;
  public:
    static Input &getInstance();
    PlayerController &getPlayerController();
    int processInput();
    void controlPlayer(PlayerController *p);
    void releasePlayer();
};

#endif
