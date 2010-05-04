#include "sound.h"
#include "Utilities/error.h"
#include <boost/lexical_cast.hpp>
#include <dirent.h>

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

char* fileToData(FILE*, BasicWAVEHeader*);
uint16_t swapends(uint16_t);
uint32_t swapends(uint32_t);
void DisplayALError(string, ALuint);
int am_big_endian();
int ends_with(string, string);
ALuint dataToBuffer(char*, BasicWAVEHeader);
ALuint filenameToBuffer(const string);

sound_resource::sound_resource(ALuint buffer) : buffer(buffer)
{
}

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

void Sound::setDir(string dirname)
{
    base_sound_directory = dirname;
}

void Sound::initSound()
{
    Error &error = Error::getInstance();

    // Open device, create context
    ALCdevice *Device = alcOpenDevice(NULL);
    ALCcontext *Context;
    Context = alcCreateContext(Device, NULL);
    alcMakeContextCurrent(Context);

    // Load sound files into sound library
    vector<string> *wav_filenames = get_wav_filenames();
    if (!wav_filenames) return;
    for (vector<string>::iterator it = wav_filenames->begin();
         it != wav_filenames->end();
         it++)
    {
        ALuint buf = filenameToBuffer(base_sound_directory + *it);
        sound_resource *a = new sound_resource(buf);
        sound_library.insert(std::pair<const string, sound_resource*>(*it, a));
        error.log(SOUND, TRIVIAL, "loaded sound from " + *it + " into buffer #" + boost::lexical_cast<string>(buf) + "\n");
    }
    delete wav_filenames;

    initialized = true;
    return;
}

void Sound::registerListener(const Camera *c)
{
    camera = c;
}

void Sound::updateListener()
{
    Error &error = Error::getInstance();
    if (!initialized)
    {
        error.log(SOUND, IMPORTANT, "Can't update listener, sound is not initialized");
        return;
    }
    if(!camera)
    {
        error.log(SOUND, IMPORTANT, "Can't update listener, no listener is registered");
        return;
    }

    const Vec3f &p = camera->getPos();
    const Vec3f &t = camera->getTarget();
    const Vec3f &up = camera->getUp();
    const Vec3f &v = camera->getAgent()->getKinematic().vel;
    const Vec3f at = t-p;

    ALfloat pos[6] = { p[0], p[1], p[2] };
    float orient[6] = { at[0], at[1], at[2], up[0], up[1], up[2] };
    ALfloat vel[3] = { v[0], v[1], v[2] };
    ALfloat gain[1] = { 2.0 };

    alListenerfv(AL_POSITION, pos);
    alListenerfv(AL_ORIENTATION, orient);
    alListenerfv(AL_VELOCITY, vel);
    alListenerfv(AL_GAIN, gain);
}

void Sound::render()
{
    Error &error = Error::getInstance();
    if (!initialized)
    {
        error.log(SOUND, IMPORTANT, "Can't render sounds, sound not initialized\n");
        return;
    }

    World *world = &World::getInstance();

    for (vector<WorldObject>::iterator i = world->wobjects.begin(); i != world->wobjects.end(); i++)
    {
        if (!i->sobject) continue;
        i->sobject->update(&(*i));
    }

    updateListener();
}

void Sound::registerSource(WorldObject *w, SObject *s)
{
    w->sobject = s;
}

vector<string> *Sound::get_wav_filenames()
{
    Error &error = Error::getInstance();
    vector<string> *wav_filenames = new vector<string>;
    struct dirent *dirent;
    DIR *dir;
    if (base_sound_directory.empty())
    {
        error.log(SOUND, IMPORTANT, "No sound directory specified\n");
        return NULL;
    }
    if ((dir = opendir(base_sound_directory.c_str())) == NULL)
    {
        error.log(SOUND, CRITICAL, "Can't open " + base_sound_directory + "\n");
        return NULL;
    }
    while ((dirent = readdir(dir)))
    {
        if (ends_with(dirent->d_name, ".wav"))
        {
            wav_filenames->push_back(dirent->d_name);
        }
    }
    if (closedir(dir) != 0)
    {
        error.log(SOUND, CRITICAL, "Trouble closing " + base_sound_directory + "\n");
        return NULL;
    }

    return wav_filenames;
}

sound_resource *Sound::lookup(const string name)
{
    return sound_library[name];
}

ALuint filenameToBuffer(const string filename)
{
    Error &error = Error::getInstance();

    if (filename.empty()) 
    {
        error.log(SOUND, CRITICAL, "Not passed a valid filename.\n");
        exit(0);
    }
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        error.log(SOUND, CRITICAL, "Failed to open file: " + filename + ".\n");
        exit(0);
    }

    BasicWAVEHeader header;
    char* data = fileToData(file, &header);
    fclose(file);
    if (!data)
    {
        error.log(SOUND, CRITICAL, "Failed to read file.\n");
        exit(0);
    }
    ALuint buffer = dataToBuffer(data, header);
    free(data);
    if (!buffer)
    {
        error.log(SOUND, CRITICAL, "Failed to create buffer.\n");
        exit(0);
    }
    return buffer;
}

char* fileToData(FILE *file, BasicWAVEHeader* header)
{
    char* buffer = NULL;
  
    if (fread(header, sizeof(BasicWAVEHeader), 1, file))
    {
        header->riffSize = swapends(header->riffSize);
        header->fmtSize = swapends(header->fmtSize);
        header->format = swapends(header->format);
        header->channels = swapends(header->channels);
        header->samplesPerSec = swapends(header->samplesPerSec);
        header->bytesPerSec = swapends(header->bytesPerSec);
        header->blockAlign = swapends(header->blockAlign);
        header->bitsPerSample = swapends(header->bitsPerSample);
        header->dataSize = swapends(header->dataSize);
        // these things *must* be valid with this basic header
        if (!(memcmp("RIFF", header->riff, 4) ||
              memcmp("WAVE", header->wave, 4) ||
              memcmp("fmt ", header->fmt, 4)  ||
              memcmp("data", header->data, 4)))
        {
            buffer = (char*)malloc(header->dataSize);
            if (buffer)
            {
                if (fread(buffer, header->dataSize, 1, file))
                {
                    return buffer;
                }
                free(buffer);
            }
        }
    }
    return NULL;
}

ALuint dataToBuffer(char* data, BasicWAVEHeader header)
{
    int errno;
    ALuint buffer = 0;
  
    ALuint format = 0;
    switch (header.bitsPerSample){
      case 8:
        format = (header.channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
        break;
      case 16:
        format = (header.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        break;
      default:
        return 0;
    }
  
    alGetError();
    alGenBuffers(1, &buffer);
    if ((errno = alGetError()) != AL_NO_ERROR)
    {
        DisplayALError("alGenBuffers: ", errno);
        return 0;
    }
    if (am_big_endian())
    {
        char *b = (char*) malloc(header.dataSize);
        for (unsigned int i=0; i < header.dataSize; i+=2) {
            swab(data+i, b+i, 2);
        }
        memcpy(data, b, header.dataSize);
        free(b);
    }
    alBufferData(buffer, format, data, header.dataSize, header.samplesPerSec);
    return buffer;
}

uint16_t swapends(uint16_t u)
{
    if (!am_big_endian()) return u;
    uint16_t v;
    swab(&u, &v, 2);
    return v;
}

uint32_t swapends(uint32_t u)
{
    if (!am_big_endian()) return u;
    unsigned char byte[4];
    byte[0] = (u << 24) >> 24;
    byte[1] = (u << 16) >> 24;
    byte[2] = (u << 8) >> 24;
    byte[3] = (u << 0) >> 24;
    return (byte[0]<<24)+(byte[1]<<16)+(byte[2]<<8)+byte[3];
}

void DisplayALError(string msg, ALuint errno)
{
    Error &error = Error::getInstance();
    const char *errMsg = NULL;
    switch (errno)
    {
        case AL_NO_ERROR:     errMsg = "None"; 
                              break;
        case AL_INVALID_NAME: errMsg = "Invalid name.";
                              break;
        case AL_INVALID_ENUM: errMsg = "Invalid enum.";
                              break;
        case AL_INVALID_VALUE:errMsg = "Invalid value.";
                              break;
        case AL_INVALID_OPERATION:errMsg = "Invalid operation.";
                              break;
        // Etc.
        default:              errMsg = "Unknown error.";
                              break;
    }
    error.log(SOUND, CRITICAL, msg + errMsg);
} 

int am_big_endian()
{
    long one= 1;
    return !(*((char *)(&one)));
}

int ends_with(string a, string b)
{
    int spot = a.rfind(b);
    return (spot+b.size() == a.size());
}
