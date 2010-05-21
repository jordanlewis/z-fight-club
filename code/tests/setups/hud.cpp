#include "Engine/setup.h"
#include "Utilities/vec3f.h"
#include "Graphics/hud.h"

void testSetup()
{
    Hud &hud = Hud::getInstance();

    Vec3f speedometer_pos = Vec3f(0.0f, 0.0f, 0.0f);
    Speedometer speedometer(speedometer_pos, humanagent);
    hud.widget.push_back(speedometer);
}
