#include "Engine/setup.h"
#include "Physics/physics.h"
#include "Agents/player.h"
#include "Engine/input.h"
#include "Engine/world.h"
#include "Utilities/vec3f.h"
#include "Agents/ai.h"
#include "Graphics/hud.h"

void testSetup()
{
    World &world  = World::getInstance();
    Physics &physics  = Physics::getInstance();
    Input &input  = Input::getInstance();
    AIManager &ai = AIManager::getInstance();
    Sound &sound  = Sound::getInstance();
		Hud &hud = Hud::getInstance();

		Vec3f speedometer_pos = Vec3f(0.0f, 0.0f, 0.0f);
		Speedometer speedometer(speedometer_pos, humanagent);
		hud.widget.push_back(speedometer);
}
