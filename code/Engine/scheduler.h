#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "allclasses.h"

#define SC_CLIENT_UPDATE_FREQ_SECONDS .05
#define SC_SERVER_UPDATE_FREQ_SECONDS .05

typedef enum
{
    SETUP,
    COUNTDOWN,
    RACE,
    SOMEONE_DONE,
    PLAYER_DONE,
    ALL_DONE,
    PAUSE
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

  public:
    RaceState_t raceState;
    double timeStarted;
    void welcomeScreen();
    void setupLoopForever();
    void soloLoopForever();
    void clientLoopForever();
    void serverLoopForever();

    static Scheduler &getInstance();
};

#endif
