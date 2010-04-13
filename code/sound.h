#ifndef SOUND_H
#define SOUND_H

#include <string>
#include <map>
#include <queue>
using namespace std;

// use strings to uniquely identify playable sounds that may have been loaded by the server
// sound_handle uniquely identifies a sound instance in the queue, scheduled for play
typedef int sound_handle;

class sound_resource
{
    // low level metadata needed to actually play the sound
    // eg. pointer to sound data in memory, sound format details
};

class playing_sound
{
  private:
    sound_handle id;
    sound_resource* s;
    double start_time;
    int repeat;
  public:
};
// Need to define less<playing_sound>

class server
{
  private:
    
    // map<Key, Data, Compare, Alloc>
    map<const string, sound_resource*> sound_library; // load_sound adds items here
    // priority_queue<T, Sequence, Compare>
    priority_queue<playing_sound> sound_queue; // play and loop add here
                                               // stop and stopall remove items
                                               // pause and pause all do... different things
    int go();
    // for each sound in priority_queue with start_time < now()
    // pop it off the queue
    // play it
    // if repeat == -1, requeue at start_time+resource.length
    // if repeat > 1, requeue at start_time+resource.length, decrementing repeat
    // if repeat == 1 or repeat == 0, do nothing

  public:
    // takes directory to scan for sound files
    // will probably preload all of them for simplicity
    server(string);

    // load sound identified by the string, add to table, returns success or failure
    int load_sound(string);

    // look up string in table, calls load_sound if not found
    // play sound some number of times, -1 means indefinitely
    // returns -1 on failure, or positive int as handle to be used to stop/pause the same sound
    sound_handle loop(string, double start_time, int repeat);

    // calls loop with repeat==1
    sound_handle play(string, double start_time);

    // stop particular sound
    int stop(sound_handle);
    // stop all sounds
    int stopall();

    // pause particular sound
    int pause(sound_handle);
    // pause all sounds
    int pauseall();
};

#endif
