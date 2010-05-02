#include "sobject.h"

SObject::SObject(string soundName, double startTime, bool loop) : startTime(startTime), loop(loop)
{
    Sound &sound = Sound::getInstance();
    sr = sound.lookup(soundName);
    alGenSources(1, &source);
    alSourcei(source, AL_LOOPING, loop);
    alSourceQueueBuffers(source, 1, &(sr->buffer));
    float size, bits, frequency, channels;
    alGetBufferf(sr->buffer, AL_SIZE, &size);
    alGetBufferf(sr->buffer, AL_BITS, &bits);
    alGetBufferf(sr->buffer, AL_FREQUENCY, &frequency);
    alGetBufferf(sr->buffer, AL_CHANNELS, &channels);
    duration = size * sizeof(char) / bits / frequency / channels;
}

void SObject::update(WorldObject *host)
{
    double now = GetTime();

    // if it should be done, stop it
    if (!loop && (now > (startTime + duration)))
    {
        alSourceStop(source);
        alSourceUnqueueBuffers(source, 1, &sr->buffer);
        alDeleteSources(1, &source);
        playing = false;
        return;
    }

    // use pos, vel from host to (re)configure the source 
    Vec3f p = host->getPos();
    float pos[3] = { p[0], p[1], p[2] };
    alSourcefv(source, AL_POSITION, pos);

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

// alDeleteBuffers(1, &buffer);
