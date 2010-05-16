#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "world.h"
#include "input.h"
#include "Graphics/graphics.h"
#include "Physics/physics.h"
#include "Sound/sound.h"
#include "Agents/ai.h"
#include "Network/client.h"
#include "Network/server.h"
#include "Utilities/error.h"

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
    void soloLoopForever();
    void clientLoopForever();
    void serverLoopForever();

    static Scheduler &getInstance();
};

#endif
