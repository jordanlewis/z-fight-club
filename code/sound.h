#ifndef SOUND_H
#define SOUND_H

namespace sound{

    // sound_descriptor keeps track of a playable sound 
    // sound_handle keeps track of a instance playing or waiting to be played
    typedef int sound_descriptor;
    typedef int sound_handle;

    class server {
    private:
	table<sound_descriptor, sound_resource> sound_library; // register adds items here
	priority_queue<handle, sound_descriptor, attime> sound_queue; // play and loop add here
                                                                      // stop and stopall remove items
                                                                      // pause and pause all do... different things

    public:
	sound_descriptor register(string);
	sound_handle play(sound_descriptor, attime); // -1 is failure, positive is a handle to be used for stopping same sound
	sound_handle loop(sound_descriptor, repeat); // play sound some number of times, negative -1 for indefinitely

	int stop(sound_handle); // stop sound playing as handle
	int stopall(); // stop all sounds

	int pause(sound_handle); // stop sound playing as handle
	int pauseall(); // pause all sounds
    };

    class sound_resource {
	// low level metadata needed to actually play the sound
    };

}
#endif
