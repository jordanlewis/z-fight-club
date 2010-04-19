#ifndef WORLD_H
#define WORLD_H

#include <ode/ode.h>

#include <vector>
#include "Agents/agent.h"
#include "Graphics/camera.h"
#include "Graphics/polygon.h"
#include "Parser/track-parser.h"

class World
{
  private:
    std::vector<Polygon> environment; /* The Environmental geometry */

    // Prevent construction, destruction, copying, or assignment in singleton
    World();
    ~World();
    World(const World&);
    World& operator=(const World&);

    static World _instance;
    Camera camera;
    TrackData_t *track;
  public:
    static float xMax;   /* XXX what are these? */
    static float zMax;
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<Agent *> agents; /* the agents in the world */

    void registerAgent(Agent *agent);
    void loadTrack(char *file);
    static World &getInstance();

    friend class Graphics;
};

#endif
