#include "sound.h"
#include "../Utilities/error.h"
#include <boost/lexical_cast.hpp>

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <AL/al.h>
#  include <AL/alc.h>
#endif

Sound Sound::_instance;

Sound::Sound()
{
    // error = Error::getInstance();
    initialized = false;
}

Sound::~Sound()
{
}

Sound &Sound::getInstance()
{
    return _instance;
}

void Sound::setDir(string d)
{
    base_sound_directory = d;
}

void Sound::initSound()
{
    ALCdevice *Device = alcOpenDevice(NULL);
    ALCcontext *Context;
    Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);
    initialized = true;
}

void Sound::update_listener(const Camera &c)
{
    if (!initialized)
    {
        Error &error = Error::getInstance();
        error.log(SOUND, IMPORTANT, "Can't update listener, sound not initialized\n");
        return;
    }
    const Vec3f &p = c.getPos();
    const Vec3f &t = c.getTarget();
    const Vec3f &up = c.getUp();
    const Vec3f &v = c.getAgent()->getKinematic().vel;
    const Vec3f at = t-p;

    ALfloat pos[6] = { p[0], p[1], p[2] };
    float orient[6] = { at[0], at[1], at[2], up[0], up[1], up[2] };
    ALfloat vel[3] = { v[0], v[1], v[2] };

    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, orient);
    alListenerfv(AL_VELOCITY, vel);
}

void Sound::render()
{
    Error &error = Error::getInstance();
    if (!initialized)
    {
        error.log(SOUND, IMPORTANT, "Can't render sounds, sound not initialized\n");
        return;
    }
    double now = GetTime();
    string a = "I really should make some noise, since it's " + boost::lexical_cast<string>(now) + "\n";
    error.log(SOUND, TRIVIAL, a.c_str());
}
