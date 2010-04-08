#ifndef SOUND_H
#define SOUND_H

namespace sound
{

    // use strings to uniquely identify playable sounds that may have been loaded by the server
    // sound_handle uniquely identifies a sound instance in the queue, scheduled for play
    typedef int sound_handle;

    class server
    {
      private:
        
        table<string, sound_resource> sound_library; // load_sound adds items here
        priority_queue<handle, sound_resource, start_time> sound_queue; // play and loop add here
                                                                      // stop and stopall remove items
                                                                      // pause and pause all do... different things

      public:
        // takes directory to scan for sound files
        // will probably preload all of them for simplicity
        server(string);

        // load sound identified by the string, add to table, returns success or failure
        int load_sound(string);

        // look up string in table, calls load_sound if not found
        // play sound some number of times, -1 means indefinitely
        // returns -1 on failure, or positive int as handle to be used to stop/pause the same sound
        sound_handle loop(string, start_time, repeat);

        // calls loop with repeat==1
        sound_handle play(string, start_time);

        // stop particular sound
        int stop(sound_handle);
        // stop all sounds
        int stopall();

        // pause particular sound
        int pause(sound_handle);
        // pause all sounds
        int pauseall();
    };

    class sound_resource
    {
        // low level metadata needed to actually play the sound
        // eg. pointer to sound data in memory, sound format details
    };

}
#endif
