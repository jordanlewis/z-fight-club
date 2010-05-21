#ifndef SOUND_H
#define SOUND_H

#include "allclasses.h"
#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <stdint.h>
using namespace std;

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#else
#  include <AL/al.h>
#  include <AL/alc.h>
#endif

typedef struct {
    char  riff[4]; // 'RIFF'
    uint32_t riffSize;      // little
    char  wave[4]; // 'WAVE'

    char  fmt[4];  // 'fmt '
    uint32_t fmtSize;       // little
    uint16_t format;        // little
    uint16_t channels;      // little
    uint32_t samplesPerSec; // little
    uint32_t bytesPerSec;   // little
    uint16_t blockAlign;    // little
    uint16_t bitsPerSample; // little

    char  data[4]; // 'data'
    uint32_t dataSize;      // little
} BasicWAVEHeader;

class sound_resource
{
  private:
    // low level metadata needed to actually play the sound
    // eg. pointer to sound data in memory, sound format details
  public:
    ALuint buffer; // maybe just use the buffer identifier instead of this whole class. we'll see.
    sound_resource(ALuint buffer);
};

class Sound
{
  private:
    Sound();
    ~Sound();
    Sound(const Sound&);
    Sound &operator=(const Sound &);
    static Sound _instance;
    string base_sound_directory;
    bool initialized;
    const Camera *camera;
    vector<string> *get_wav_filenames();
    map<const string, sound_resource*> sound_library;
    World *world;
    Error *error;

    ALuint dataToBuffer(char*, BasicWAVEHeader);
    ALuint filenameToBuffer(const string);
    char* fileToData(FILE*, BasicWAVEHeader*);
    int am_big_endian();
    int ends_with(string, string);
    uint16_t swapends(uint16_t);
    uint32_t swapends(uint32_t);
    void DisplayALError(string, ALuint);

  public:

    // used anywhere we need to interact with sound
    // returns reference to the single instance of the Sound class
    static Sound &getInstance();

    // used by Engine/racer.cpp
    // configures the base directory for sound assets, otherwise, cwd is assumed
    void setDir(const string);

    // used by Engine/racer.cpp
    // does the stuff that should be done exactly once like open devices and load sound assets
    void initSound();

    // used by Engine/scheduler.cpp
    // do some stuff whenever we get a chance
    // eg. (re)fill buffers, clear things that are done
    // (sometimes?) update listener and source locations, velocities, etc.
    void render();

    // when we switch camera (modes), switch listener
    void registerListener(const Camera*);
    // periodically update based on camera movement
    void updateListener();

    // used by tests/setups/sound.cpp
    // I made an object, attach this sound information to it
    void registerSource(WorldObject*, SObject*);

    sound_resource *lookup(const string);
};

// sounds come from various sources:
// 1) an agent moving around the track
// 2) a fixed object (singing rock)
// 3) an interaction between two objects (car hits wall)
// 4) metatron (voices in my head, pre-mixed in stereo)

// When a WorldObject is created, it can have a SObject associated with it
// the SObject will start by just having a looping sound, always playing
// with a particular gain.

// When an interaction happens... physics should add a new world object
// with sound, maybe graphics, a velocity. for instance, skidding might
// match the car, but leave trails, a collision will stay at the point
// of impact, and include some pretty explosions

// When metatron has something to say, Sound will take care of it
// directly, without a location or velocity or WorldObject

#endif
