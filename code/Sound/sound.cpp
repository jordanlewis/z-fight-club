#include "sound.h"
#include "Utilities/error.h"
#include <boost/lexical_cast.hpp>
#include <al.h>
#include <alc.h>

Sound Sound::_instance;

Sound::Sound()
{
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
}

void Sound::update_listener(Camera c)
{
}


void Sound::render()
{
    double now = GetTime();
    Error &error = Error::getInstance();
    string a = "I really should make some noise, since it's " + boost::lexical_cast<string>(now) + "\n";
    error.log(SOUND, TRIVIAL, a.c_str());
}
