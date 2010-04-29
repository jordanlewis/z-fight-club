#ifndef WORLD_H
#define WORLD_H

#include <ode/ode.h>

#include <vector>
#include "Agents/agent.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Graphics/camera.h"
#include "Graphics/polygon.h"
#include "Parser/track-parser.h"

class PGeom;

class WorldObject
{
  public:
    PGeom *pobject;
    GObject *gobject;
    WorldObject(PGeom * pobject, GObject * gobject);
};

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
  public:
    Camera camera;
    TrackData_t *track;
    static float xMax;   /* XXX what are these? */
    static float zMax;
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<Agent *> agents; /* the agents in the world */
    std::vector<WorldObject> objects; /* the objects in the world */

    void addObject(PGeom *pobject, GObject *gobject);
    void registerAgent(Agent &agent);
    void loadTrack(const char *file);
    const TrackData_t * getTrack();
    static World &getInstance();

    friend class Graphics;
};

#endif
