#include "Engine/setup.h"
#include "Utilities/vec3f.h"
#include "Graphics/hud.h"

void testSetup()
{
    World &world  = World::getInstance();

    Vec3f speedometer_pos = Vec3f(0.0f, 0.0f, 0.0f);
    Speedometer *speedometer = new Speedometer(speedometer_pos, NULL);
    world.addWidget(speedometer);
}
