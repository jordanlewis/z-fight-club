#ifndef SOUND_H
#define SOUND_H

#include "../Engine/world.h"
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <string>
#include <map>
#include <queue>
using namespace std;

#define SM_VOICES 8

// use strings to uniquely identify playable sounds that may have been loaded by the server
// sound_handle uniquely identifies a sound instance in the queue, scheduled for play
typedef int sound_handle;

class sound_resource;
// low level metadata needed to actually play the sound
// eg. pointer to sound data in memory, sound format details

typedef struct
{
        Sint16  *data;
        int     length;
        int     position;
        int     l_vol;
        int     r_vol;
} SM_voice;

class Sound;

class playing_sound
{
  protected:
    sound_handle id;
    const sound_resource* s;
    double start_time;
    int repeat;
  public:
    bool operator<(const playing_sound &s) const;
    playing_sound(const sound_resource* s, double start_time, int repeat);
    friend class Sound;
    friend ostream &operator<<(ostream&, const playing_sound&);
  private:
    static sound_handle next_handle;
};

class Sound
{
  private:
    Sound();
    ~Sound();
    Sound(const Sound&);
    Sound &operator=(const Sound &);
    void load_sound(const string sound_name);
    SM_voice myvoices[SM_VOICES];
    static int cur_voice;  
    static Sound _instance;

    bool initialized;    /* !<is SDL audio ready to go */
    SDL_AudioSpec audiospec;
    string dir; // "tests/sounds/";

  public:
    void initSound();
    // needs directory to scan for sound files
    // will probably preload all of them for simplicity
    void setDir(const char *dir);
    const sound_handle schedule_sound(const string sound_name, double start_time, Vec3f location);
    // ignoring location information, playing equally loud in both channels.
    // no support for looping, just schedule again if you want it again.
    // no support for stopping a particular sound, or sound at all. too bad.
    // if the sound you asked for isn't found, it just doesn't play.

    void process_queue();
    static Sound &getInstance();
    void mixer(Uint8 *stream, int len);
    void play(const playing_sound ps);

  private:
    // map<Key, Data, Compare, Alloc>
    map<const string, const sound_resource*> sound_library; // load_sound adds items here

    // priority_queue<T, Sequence, Compare>
    priority_queue<playing_sound> sound_queue; // play and loop add here
                                               // stop and stopall remove items
                                               // pause and pause all do... different things

/* Not sure what of this, if any, I will keep    

    sound_handle loop(const string, double start_time, int repeat);
    int stop(sound_handle);
    int stopall();
    int pause(sound_handle);
    int pauseall();
*/

};

#endif
