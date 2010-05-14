#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Agents/ai.h"
#include "Sound/sound.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();
    error.on(SOUND);

    World &world = World::getInstance();
    Physics &physics  = Physics::getInstance();
    AIManager &ai = AIManager::getInstance();
    Sound &sound = Sound::getInstance();
    Input &input = Input::getInstance();

}
