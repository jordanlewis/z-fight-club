#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Sound/sound.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();
    error.on(SOUND);

    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Sound &sound = Sound::getInstance();
    Input &input = Input::getInstance();


    PlaneInfo info = PlaneInfo(0, 1, 0, 0);
    new PGeom(&info, physics.getOdeSpace());

    Vec3f pos = Vec3f(85, 5, 20);
    Agent *agent = new Agent(pos, M_PI);

    world.addAgent(*agent);
    /* Agents will become world objects, right?
     * then we'll someone give them useful SObject components */
    // Sound::register_source(...);
    world.camera = Camera(THIRDPERSON,agent);
    /* The microphones will get their location, orientation, and velocity from the camera */
    sound.update_listener(world.camera);

    /* Instantiate a playercontroller to handle input -> steering conversion for
     * this agent */
    PlayerController *p = new PlayerController(*agent);
    /* Tell input to send input to that playerController */
    input.controlPlayer(*p);
}
