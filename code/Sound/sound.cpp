/* Code inspired by "Simple Mixer", by
 David Olofson <david@olofson.net>
 http://olofson.net/examples.html
 http://olofson.net/download/simplemixer-1.1.tar.gz
*/

#include "sound.h"
#include "Utilities/defs.h"

#define FREQ     44100
#define CHANNELS 2
#define SAMPLES  512

sound_handle playing_sound::next_handle = 0;
int Sound::cur_voice = 0;

bool playing_sound::operator< (const playing_sound &s) const
{
    return s.start_time < start_time;
}

playing_sound::playing_sound(const sound_resource* s, double start_time, int repeat) :
    s(s),
    start_time(start_time),
    repeat(repeat)
{
    id = next_handle++;
}

class sound_resource
{
    // low level metadata needed to actually play the sound
    // eg. pointer to sound data in memory, sound format details
  protected:
    Uint8 *data;
    Uint32 length;
    sound_resource()
    {
        data = NULL;
        length = 0;
    };
  public:
    friend class Sound;
    friend ostream& operator<<(ostream &os, const playing_sound &ps);
};

ostream& operator<<(ostream &os, const playing_sound &ps)
{
    os.precision(20);
    os << &ps.id << " id: " << ps.id << endl
       << " start_time: " << ps.start_time << endl
       // << "     repeat: " << ps.repeat << endl
       << "          s: " << ps.s << endl
       << "     length: " << ps.s->length << endl
       << "       data: " << (int) ps.s->data << endl; 
    return os;
}

Sound Sound::_instance;

Sound::Sound()
{
    initialized = false;
}

Sound::~Sound()
{
    // empty out hash and priority_queue?
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}


static void sm_mixer(void *unused, Uint8 *stream, int len)
{
    Sound::getInstance().mixer(stream, len);
}

void Sound::mixer(Uint8 *stream, int len)
{
    // use voices array within Sound instance

    Sint16 *buf = (Sint16 *)stream;

    /* Clear the buffer */
    memset(buf, 0, len);

    /* 2 channels * 2 bytes/sample = bytes/frame */
    len /= 4;

    /* For each voice... */
    for(int vi = 0; vi < SM_VOICES; ++vi)
    {
        SM_voice *v = &myvoices[vi];
        if(!v->data)
            continue;

        /* For each sample... */
        for(int s = 0; s < len; ++s)
        {
            if(v->position >= v->length)
            {
                v->data = NULL;
                break;
            }
            buf[s * 2] += v->data[v->position] * v->l_vol >> 8;
            buf[s * 2 + 1] += v->data[v->position] * v->r_vol >> 8;
            ++v->position;
        }
    }
    return;
}

void Sound::initSound()
{
    /* set up SDL */
    SDL_Init(0);
    
    SDL_AudioSpec audiospec_desired;

    /* if I use vectors for potential and playing sounds, I shouldn't need these
    memset(sounds, 0, sizeof(sounds));
    memset(myvoices, 0, sizeof(myvoices));
    */

    SDL_InitSubSystem(SDL_INIT_AUDIO);

    audiospec_desired.freq = FREQ;
    audiospec_desired.format = AUDIO_S16SYS;
    audiospec_desired.channels = CHANNELS;
    audiospec_desired.samples = SAMPLES;
    audiospec_desired.callback = sm_mixer;
    SDL_OpenAudio(&audiospec_desired, &audiospec);
    SDL_PauseAudio(0);

    // load all sounds in hard-coded directory
    load_sound("808-bassdrum.wav");
    load_sound("808-clap.wav");
    load_sound("808-cowbell.wav");
    load_sound("808-hihat.wav");
}

const sound_handle Sound::schedule_sound(const string sound_name, double start_time, Vec3f location)
{
    // "sound" should be found in the sound_library
    // calculate vector from camera to location to determine where to play sound
    // push the sound_resource onto the sound_queue
    // return the sound_handle, so this can be paused (requeued with HUGE_VAL
    // start_time) or stopped (dequeued) later

    const sound_resource* s = sound_library[sound_name];
    playing_sound ps(s, start_time, 1);
    sound_queue.push(ps);
    return ps.id;
}

void Sound::play(const playing_sound ps)
{
    // the example gets voice, sound, left volume, right volume
    // start with
    //  myvoices[0]
    //  sound comes from ps
    //  left volume = 1
    //  right volume = 1
    if (cur_voice == SM_VOICES) cur_voice = 0;
    int i = cur_voice++;

    // sanity check on ps, the only input
    if (ps.s == NULL) return;

    // stop current voice
    myvoices[i].data = NULL;

    // describe new voice 
    myvoices[i].length = ps.s->length / 2; // not sure why
    myvoices[i].position = 0;
    myvoices[i].l_vol = 256.0;
    myvoices[i].r_vol = 256.0;

    // start playing?
    myvoices[i].data = (Sint16*) ps.s->data;
}

void Sound::process_queue()
{
    double now = GetTime();
    while (1)
    {
        if (sound_queue.empty()) return;
        const playing_sound &ps = sound_queue.top();
        if (ps.start_time < now)
        {
            Sound::play(ps);
            sound_queue.pop();
        }
        else
        {
            return;
        }
    }
}

Sound &Sound::getInstance()
{
    return _instance;
}

void flip_endian(Uint8 *data, int length)
{
        int i;
        for(i = 0; i < length; i += 2)
        {
                int x = data[i];
                data[i] = data[i + 1];
                data[i + 1] = x;
        }
}

void Sound::setDir(const char *d)
{
    dir = d;
}

void Sound::load_sound(const string sound_name)
{
    const char *file_name = (dir+sound_name).c_str();
    int failed = 0;
    SDL_AudioSpec spec;

    // prepare a place
    // create an empty sound_resource object
    sound_resource *sr = new sound_resource();
    if (sr == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for sound!\n");
        failed = 1;
    }
    else if(SDL_LoadWAV(file_name, &spec, &sr->data, &sr->length) == NULL)
    {
        fprintf(stderr, "Unable to load sound!\n");
        failed = 1;
    }
    else if(spec.freq != FREQ)
    {
        fprintf(stderr, "WARNING: File '%s' is not %d Hz.\n",
                file_name, FREQ);
        failed = 1;
    }
    else if(spec.channels != 1)
    {
        fprintf(stderr, "Only mono sounds are supported!\n");
        failed = 1;
    }

    switch(spec.format)
    {
      case AUDIO_S16LSB:
      case AUDIO_S16MSB:
        if(spec.format != AUDIO_S16SYS)
            flip_endian(sr->data, sr->length);
        break;
      default:
        fprintf(stderr, "Unsupported sample format!\n");
        failed = 1;
        break;
    }

    // free memory if there was a failure
    if(failed)
    {
        SDL_FreeWAV(sr->data);
        sr->length = NULL;
        delete sr;
        return;
    }
    else
    {
        sound_library.insert(pair<const string, const sound_resource*>(sound_name,sr));
    }
    return;
}
