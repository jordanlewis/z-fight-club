#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();
    error.on(NETWORK);
}
