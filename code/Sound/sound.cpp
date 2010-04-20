/* Code inspired by "Simple Mixer", by
 David Olofson <david@olofson.net>
 http://olofson.net/examples.html
 http://olofson.net/download/simplemixer-1.1.tar.gz
*/

#include "sound.h"
#include "Utilities/defs.h"

#define FREQ		44100
#define CHANNELS	2
#define SAMPLES		512

sound_handle playing_sound::next_handle = 0;

bool playing_sound::operator< (const playing_sound &s) const
{
    return s.start_time < start_time;
}

playing_sound::playing_sound(const sound_resource* s, double start_time, int repeat) :
    s(s),
    start_time(start_time),
    repeat(repeat)
{
    this->id = next_handle++;
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
};

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

static void sm_mixer(void *ud, Uint8 *stream, int len)
{
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
    SDL_OpenAudio(&audiospec_desired, &this->audiospec);
    SDL_PauseAudio(0);

    // load all sounds in hard-coded directory
    load_sound("808-bassdrum.wav");
    load_sound("808-clap.wav");
    load_sound("808-cowbell.wav");
    load_sound("808-hihat.wav");
    cerr << "done loading sounds" << endl;
}

const sound_handle Sound::schedule_sound(const string sound_name, double start_time, Vec3f location)
{
    // "sound" should be found in the sound_library
    // calculate vector from camera to location to determine where to play sound
    // push the sound_resource onto the sound_queue
    // return the sound_handle, so this can be paused (requeued with HUGE_VAL
    // start_time) or stopped (dequeued) later

    const sound_resource* s = this->sound_library[sound_name];
    playing_sound ps(s, start_time, 1);
    this->sound_queue.push(ps);
    return ps.id;
}

void Sound::process_queue()
{
    double now = GetTime();
    while (1)
    {
        if (sound_queue.empty()) return;
        const playing_sound &s = sound_queue.top();
        sound_queue.pop();
        if (s.start_time < now)
        {
            cerr << "should be playing " << s.id << " now" << endl;
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

void Sound::load_sound(const string sound_name)
{
        const string dir = "/tmp/";
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

	// try to get the data into the right place
        if(SDL_LoadWAV(file_name, &spec, &sr->data,
                        &sr->length) == NULL)
        {
                fprintf(stderr, "Unable to load sound!\n");
                failed = 1;
        }

	// check that the data is good
        if(spec.freq != FREQ)
                fprintf(stderr, "WARNING: File '%s' is not 44.1 kHz."
                                " Might sound weird...\n", file_name);
        if(spec.channels != 1)
        {
                fprintf(stderr, "Only mono sounds are supported!\n");
                failed = 1;
        }

	// clean up data if necessary
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
		this->sound_library.insert(pair<const string, const sound_resource*>(sound_name,sr));
        }
        return;
}
