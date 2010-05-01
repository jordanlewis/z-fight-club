#ifndef SOUND_H
#define SOUND_H

#include "../Engine/world.h"
#include "../Graphics/camera.h"
#include <string>

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

  public:

    // used anywhere we need to interact with sound
    // returns reference to the single instance of the Sound class
    static Sound &getInstance();

    // used by Engine/racer.cpp
    // configures the base directory for sound assets, otherwise, cwd is assumed
    void setDir(string);

    // used by Engine/racer.cpp
    // does the stuff that should be done exactly once like open devices and load sound assets
    void initSound();

    // used by Engine/scheduler.cpp
    // do some stuff whenever we get a chance
    // eg. (re)fill buffers, clear things that are done
    // (sometimes?) update listener and source locations, velocities, etc.
    void render();

    // when we switch camera (modes), switch listener, and periodically update since camera moves
    void update_listener(Camera); // get from that the pos, up, target, agent->velocity

    // used by tests/setups/sound.cpp
    // I made an object, attach this sound information to it
    static void register_source(WorldObject*, SObject*);

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
