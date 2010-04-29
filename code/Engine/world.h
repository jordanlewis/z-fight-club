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

/*! \brief WorldObject collects all of the possible representations of an object
 * \brief in the world.
 *
 * It isn't necessary for a WorldObject to have all of these representations;
 * a NULL pointer means "no representation".
 * i.e. Only an agent needs a valid agent; the rest will have agent=NULL.
 */
class WorldObject
{
  public:
    PGeom *pobject;
    GObject *gobject;
    // SObject *sobject; // possible future SObject for sound properties
    Agent *agent;
    WorldObject(PGeom * pobject, GObject * gobject, Agent * agent);

    Vec3f getPos();
    void getRot(Mat4x4f_t);
    void getQuat(Quatf_t);

    void draw();
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
    std::vector<WorldObject> wobjects; /* the objects in the world */

    void addObject(WorldObject obj);
    void addAgent(Agent &agent);
    void loadTrack(const char *file);
    const TrackData_t * getTrack();
    static World &getInstance();

    friend class Graphics;
};

#endif
