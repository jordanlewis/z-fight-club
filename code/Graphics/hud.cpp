#include "hud.h"
#include "Engine/world.h"
#include <SDL/SDL.h>
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
extern "C" {
        #include "Utilities/load-png.h"
}

Speedometer::Speedometer(Vec3f pos, Agent *agent)
    : Widget(), agent(agent)
{
    World &world = World::getInstance();
    this->pos = pos;
    background = LoadImage((world.assetsDir + std::string("speedometer/background.png")).c_str(), false, RGBA_IMAGE);

}

Speedometer::~Speedometer()
{}

void Speedometer::draw()
{
    DrawImage(background, pos[0], pos[1]);
}

Hud Hud::_instance;

Hud::Hud()
{}

Hud::~Hud()
{}

Hud &Hud::getInstance()
{
    return _instance;
}
