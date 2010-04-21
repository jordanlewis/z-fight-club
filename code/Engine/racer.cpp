/*
 * racer.cpp
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "math.h"
#include "scheduler.h"
#include "Physics/physics.h"
#include "Graphics/graphics.h"
#include "Sound/sound.h"
#include "input.h"

#include "Utilities/vec3f.h"
#include "Utilities/defs.h"

using namespace std;

int main(int argc, char *argv[])
{
    World &world = World::getInstance();
    if (argc > 1)
	world.loadTrack(argv[1]);

    Graphics  &graphics = Graphics::getInstance();
    Sound     &sound    = Sound::getInstance();
    Physics   &physics  = Physics::getInstance();
    Scheduler &scheduler = Scheduler::getInstance();
    Input     &input = Input::getInstance();


    graphics.initGraphics();
    sound.initSound();
    physics.initPhysics();

    
    PlaneInfo info = PlaneInfo(0, 1, 0, 10, 0, 200, 0, physics.getOdeSpace());
    new PGeom(&info);

    Vec3f pos = Vec3f(-8, 20, .3);
    Vec3f pos2 = Vec3f(30, 25, 0);
    Agent agent(pos, M_PI / 2);
    Agent agent2(pos2, -M_PI / 2);

    SteerInfo steer;
    steer.acceleration = .3;
    steer.rotation = 0;
    agent.setSteering(steer);
    steer.acceleration = 1;
    agent2.setSteering(steer);

    world.registerAgent(agent);
    physics.initAgent(agent);
    world.registerAgent(agent2);
    physics.initAgent(agent2);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController p(agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(p);

    // sound.schedule_sound("808-cowbell.wav", GetTime()+2, Vec3f(0,0,0));
    double now = GetTime();
    double start = now+1;
    for (int b = 0; b < 16; b++) // for sixteen beats
    {
        double t = b/4.0; // where each beat is a quarter of a second
        if ((b%4) == 0)
            sound.schedule_sound("808-bassdrum.wav", start+t, Vec3f(0,0,0));
        if ((b%4) == 2)
            sound.schedule_sound("808-hihat.wav", start+t, Vec3f(0,0,0));
        if ((b%8) == 4)
            sound.schedule_sound("808-clap.wav", start+t, Vec3f(0,0,0));
        if ((b%8) == 0 || (b%8) == 7)
            sound.schedule_sound("808-cowbell.wav", start+t, Vec3f(0,0,0));
    }

    scheduler.loopForever();

    return 0;
}
