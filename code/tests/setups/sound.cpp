#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Sound/sound.h"
#include "Utilities/vec3f.h"
#include "Utilities/defs.h"

void testSetup()
{
    World &world     = World::getInstance();
    Physics &physics = Physics::getInstance();
    Sound &sound     = Sound::getInstance();
    Input &input     = Input::getInstance();

    PlaneInfo info = PlaneInfo(0, 1, 0, 0, 0, 0, 0, physics.getOdeSpace());
    new PGeom(&info);

    Vec3f pos = Vec3f(82, 5, 28);
    Agent *agent = new Agent(pos, M_PI / 2);

    world.addAgent(*agent);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(*agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(*p);

    double now = GetTime();
    double start = now+1;
    for (int b = 0; b < 33; b++) // for sixteen beats
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

}
