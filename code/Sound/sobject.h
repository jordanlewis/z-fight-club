#ifndef SOBJECT_H
#define SOBJECT_H

#include "sound.h"
#include "Engine/world.h"
#include <iostream>
#include <string>
using namespace std;

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#else
#  include <AL/al.h>
#  include <AL/alc.h>
#endif

class sound_resource;
// class PObject;

class SObject
{
    // A WorldObject, like a vehicle, will have an SObject if
    // it wants to generate any sounds.
    // For now, the SObject will hold just a few details:
    // the sound_resource (which has the buffer id)
    // potentially a source handling the playing of that sound_resource
  private:
    sound_resource *sr; // pointer to a sound_resource canonically held by the sound_library
    ALuint source;
    double startTime;   // when this sound should start playing
    ALint loop;         // whether to loop or not
    bool playing;       // whether it's playing right now
    ALfloat duration;
    // could have lots of other data, like whether to fade in, out, etc.
  public:
    SObject(string soundName, double startTime, bool loop);
    void update(WorldObject*); // play, restart, stop, update kinematics, etc. as appropriate
};

#endif
