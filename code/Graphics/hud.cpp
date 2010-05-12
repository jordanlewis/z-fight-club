#include "hud.h"

Speedometer::Speedometer(Vec3f pos, Agent *agent)
    : Widget(), agent(agent)
{
    this->pos = pos;
}

void Speedometer::draw()
{
}
