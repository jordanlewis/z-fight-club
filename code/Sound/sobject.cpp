#include "sobject.h"
#include "sound.h"
#include "Engine/world.h"
#include "Utilities/error.h"
#include "Agents/agent.h"
#include <boost/lexical_cast.hpp>

SObject::SObject(string soundName, double startTime, bool loop) :
    startTime(startTime),
    loop(loop),
    error(&Error::getInstance()),
    nextSound(NULL)
{
    Sound &sound = Sound::getInstance();
    sr = sound.lookup(soundName);
    duration = 0;
    playing = false;
    if (sr)
    {
        alGenSources(1, &source);
        alSourcei(source, AL_LOOPING, loop);
        alSourceQueueBuffers(source, 1, &(sr->buffer));
        ALint size = -1, bits = -1, frequency = -1, channels = -1;
        alGetBufferi(sr->buffer, AL_FREQUENCY, &frequency);
        alGetBufferi(sr->buffer, AL_BITS, &bits);
        alGetBufferi(sr->buffer, AL_CHANNELS, &channels);
        alGetBufferi(sr->buffer, AL_SIZE, &size);
        duration = (float) size / (float) frequency / (float) channels / (bits / 8);
    }

    string msg = "created sobject(";
    msg += boost::lexical_cast<string>(sr) + ") for " + soundName + "\n";
    error->log(SOUND, TRIVIAL, msg);
}

SObject::~SObject()
{
    string msg = "destroying sobject using sound resource ";
    msg += boost::lexical_cast<string>(sr) + "\n";
    error->log(SOUND, TRIVIAL, msg);
}

void SObject::setStartTime(double newtime)
{
   startTime = newtime;
}

void SObject::update(WorldObject *host)
{
    // if this sound is broken, switch to the next one immediately
    if (!duration || !sr)
    {
        host->sobject = nextSound;
        if (nextSound) host->sobject->setStartTime(GetTime());
        this->~SObject();
        return;
    }
    double now = GetTime();

    // if it should be done, stop it
    if (!loop && (now > (startTime + duration)))
    {
        alSourceStop(source);
        alSourceUnqueueBuffers(source, 1, &sr->buffer);
        alDeleteSources(1, &source);
        playing = false;
        host->sobject = nextSound;
        if (nextSound) host->sobject->setStartTime(GetTime());
        this->~SObject();
        return;
    }

    // use pos, vel from host to (re)configure the source
    Vec3f p = host->getPos();
    float pos[3] = { p[0], p[1], p[2] };
    float gain[1] = { 2.0 };
    alSourcefv(source, AL_POSITION, pos);
    alSourcefv(source, AL_GAIN, gain);

    float vel[3] = { 0, 0, 0 };
    if (host->agent)
    {
        Vec3f v = host->agent->getKinematic().vel;
        vel[0] = v[0];
        vel[1] = v[1];
        vel[2] = v[2];
    }
    alSourcefv(source, AL_VELOCITY, vel);

    // if it's not playing, and should be, start it
    if (!playing && (now > startTime))
    {
        alSourcePlay(source);
        playing = true;
    }
}

void SObject::registerNext(SObject* n)
{
    nextSound = n;
}

// alDeleteBuffers(1, &buffer);
