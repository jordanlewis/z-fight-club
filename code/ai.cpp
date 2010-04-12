#include <vector>
#include "ai.h"
#include "vector.h"
#include "agent.h"

AIController::AIController(Agent &agent)
{
    this->agent = &agent;
}

AIController::run()
{

}

void AIManager::control(Agent &agent)
{
    AIController *newController = new AIController(agent);
    controllers.push_back(newController);
}

void AIManager::release(Agent &agent)
{
    /* is there a less ugly way of doing an iterator loop like this? */
    for (std::vector<AIController*>::iterator it = controllers.begin();
         it != controllers.end(); ++it)
    {
        /* Look through our controller array, and remove the controller that
         * has the agent with the ID of the input agent */
        if ((*it)->agent->id == agent.id)
        {
            controllers.erase(it);
            return;
        }
    }
}

void AIManager::run()
{
    for (unsigned int i = 0; i < controllers.size(); i++)
    {
        controllers[i]->run();
    }
}
