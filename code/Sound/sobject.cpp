#include "sobject.h"
#include "sound.h"
#include "Engine/world.h"
#include "Utilities/error.h"
#include "Agents/agent.h"
#include <boost/lexical_cast.hpp>

SObject::SObject(string soundName, double startTime, bool loop, float gain) :
    startTime(startTime),
    loop(loop),
    gain(gain),
    error(&Error::getInstance()),
    nextSound(NULL)
{
    static int numObjects = 0;
    id = numObjects++;
    Sound &sound = Sound::getInstance();
    sr = sound.lookup(soundName);
    duration = 0;
    playing = false;
    if (sr)
    {
        if(sr->buffer == 0) { cerr << "buffer is 0" << endl; exit(1); }
        alGetError(); /* clear error */
        alGenSources(1, &source);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error creating source" << endl;
        alSourcei(source, AL_LOOPING, loop);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error setting looping" << endl;
        alSourceQueueBuffers(source, 1, &(sr->buffer));
        if(alGetError() != AL_NO_ERROR) cerr << "- Error queing buffers" << endl;
        ALint size = -1, bits = -1, frequency = -1, channels = -1;
        alGetBufferi(sr->buffer, AL_FREQUENCY, &frequency);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error getting frequency" << endl;
        alGetBufferi(sr->buffer, AL_BITS, &bits);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error getting bits" << endl;
        alGetBufferi(sr->buffer, AL_CHANNELS, &channels);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error getting channels" << endl;
        alGetBufferi(sr->buffer, AL_SIZE, &size);
        if(alGetError() != AL_NO_ERROR) cerr << "- Error getting size" << endl;
        duration = (float) size / (float) frequency / (float) channels / (bits / 8);
    }

    double foo = GetTime();
    /*
    string msg = "+SObject";
    msg += " " + boost::lexical_cast<string>(id);
    msg += " " + boost::lexical_cast<string>(sr);
    msg += " " + boost::lexical_cast<string>(duration);
    msg += " " + boost::lexical_cast<string>(foo);
    msg += " " + soundName + "\n";
    error->log(SOUND, TRIVIAL, msg);
    */
    SOUND << TRIVIAL << "+SObject";
    SOUND << TRIVIAL << " " << id;
    SOUND << TRIVIAL << " " << sr;
    SOUND << TRIVIAL << " " << duration;
    SOUND << TRIVIAL << " " << foo;
    SOUND << TRIVIAL << " " << soundName << "\n";

}

SObject::~SObject()
{
    double foo = GetTime();
    string msg = "-SObject";
    msg += " " + boost::lexical_cast<string>(id);
    msg += " " + boost::lexical_cast<string>(sr);
    msg += " " + boost::lexical_cast<string>(duration);
    msg += " " + boost::lexical_cast<string>(foo) + "\n";
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
        // this looks terrifying, but because we have a private destructor it should be fine.
        delete this;
        return;
    }
    double now = GetTime();

    // if it should be done, stop it
    if (!loop && (now > (startTime + duration)))
    {
        alSourceStop(source);
        // alSourceUnqueueBuffers(source, 1, &sr->buffer);
        alDeleteSources(1, &source);
        playing = false;
        host->sobject = nextSound;
        if (nextSound) host->sobject->setStartTime(GetTime());
        // this looks terrifying, but because we have a private destructor it should be fine.
        delete this;
        return;
    }

    // use pos, vel from host to (re)configure the source
    Vec3f p = host->getPos();
    float pos[3] = { p[0], p[1], p[2] };
    alSourcefv(source, AL_POSITION, pos);
    alSourcef(source, AL_GAIN, gain);

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
