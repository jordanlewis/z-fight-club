#include "component.h"
#include "Utilities/defs.h"

Component::Component() : avgTime(0), lastRun(0), frequency(.00001)
{}

float Component::fps()
{
    if (avgTime != 0)
        return 1 / avgTime;
    else
        return 0;
}
bool Component::start()
{
    double time = GetTime();
    if (lastRun + 1 / frequency < time)
    {
        lastRun = time;
        return true;
    }
    else
        return false;
}

void Component::finish()
{
    double time = GetTime();
    avgTime = avgTime * .9 + (time - lastRun) * .1;
}
