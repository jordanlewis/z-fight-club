#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "allclasses.h"

#define SC_CLIENT_UPDATE_FREQ_SECONDS .01
#define SC_SERVER_UPDATE_FREQ_SECONDS .01

typedef enum
{
    COUNTDOWN,
    RACE,
    PLAYER_DONE,
    ALL_DONE
} RaceState_t;

class Scheduler
{
    World    *world;
    Graphics *graphics;
    Sound    *sound;
    Physics  *physics;
    AIManager *ai;
    Input    *input;
    Client *client;
    Server *server;
    Error *error;

    static Scheduler _instance;
    Scheduler();
    ~Scheduler();
    Scheduler(const Scheduler&);
    Scheduler &operator=(const Scheduler&);
    unsigned char profilerclock;

  public:
    RaceState_t raceState;
    double timeStarted;
    void welcomeScreen();
    void soloLoopForever();
    void clientLoopForever();
    void serverLoopForever();

    static Scheduler &getInstance();
};

#endif
