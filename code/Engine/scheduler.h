#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include "allclasses.h"

#define SC_CLIENT_UPDATE_FREQ_SECONDS .05 /*!< minimum elapsed time before sending update to server */ 
#define SC_SERVER_UPDATE_FREQ_SECONDS .05 /*!< minimum elapsed time before sending update to client */ 

/*! RaceState_t enumeration
 *
 * The RaceState_t type is used by *some* of the game loops.
 */
typedef enum
{
    SETUP,
    WAITING,
    COUNTDOWN,
    RACE,
    SOMEONE_DONE,
    PLAYER_DONE,
    ALL_DONE,
    PAUSE
} RaceState_t;

/*! Scheduler singleton
 *
 * The Scheduler singleton runs the main loops.
 */
class Scheduler
{
    World    *world; /*!< pointer to world singleton */
    Graphics *graphics; /*!< pointer to graphics singleton */
    Sound    *sound; /*!< pointer to sound singleton */
    Physics  *physics; /*!< pointer to physics singleton */
    AIManager *ai; /*!< pointer to AI singleton */
    Input    *input; /*!< pointer to input singleton */
    Client *client; /*!< pointer to network client singleton */
    Server *server; /*!< pointer to network server singleton */
    Error *error; /*!< pointer to error singleton */

    static Scheduler _instance; /*!< crazy singleton nonsense */
    Scheduler(); /*!< private constructor blocks creation */
    ~Scheduler(); /*!< private constructor blocks deletion */
    Scheduler(const Scheduler&); /*!< private copy constructor blocks copy */
    Scheduler &operator=(const Scheduler&); /*!< private assignment constructor blocks use */

  public:
    RaceState_t raceState; /*!< current phase of game */
    double timeStarted; /*!< 1 second after inputSetupMenu finishes creating menu */
    void setupLoopForever(); /*!< process user input while setup menu is shown */
    void soloLoopForever(); /*!< process user input and display game in solo play mode */
    void clientLoopForever(); /*!< process user input, server input, and display game in network play mode */
    void serverLoopForever(); /*!< process client input and display game in server mode */

    static Scheduler &getInstance(); /*!< return reference to Scheduler singleton */
};

#endif
