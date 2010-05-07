#ifndef WORLD_H
#define WORLD_H

#include <ode/ode.h>

#include <vector>
#include <string>
using namespace std;

#include "Agents/agent.h"
#include "Physics/pobject.h"
#include "Graphics/gobject.h"
#include "Graphics/camera.h"
#include "Graphics/polygon.h"
#include "Sound/sobject.h"
#include "Parser/track-parser.h"

class PGeom;
class SObject;

/*! \brief WorldObject collects all of the possible representations of an object
 * \brief in the world.
 *
 * It isn't necessary for a WorldObject to have all of these representations;
 * a NULL pointer means "no representation".
 * i.e. Only an agent needs a valid agent; the rest will have agent=NULL.
 */
class WorldObject
{
  private:
    Vec3f pos;
  public:
    PGeom *pobject;
    GObject *gobject;
    SObject *sobject;
    Agent *agent;
    WorldObject(PGeom * pobject, GObject * gobject,
		SObject * sobject, Agent * agent);

    Vec3f getPos();
    void setPos(Vec3f position);
    void getRot(Mat4x4f_t);
    void getQuat(Quatf_t);

    void draw();
};

//What mode are we running our game in?
typedef enum {
    SOLO = 0,
    CLIENT = 1,
    SERVER = 2
} RunType_t;

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
    RunType_t runType;
    Camera camera;
    TrackData_t *track;
    static float xMax;   /* XXX what are these? */
    static float zMax;
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<Agent *> agents; /* the agents in the world */
    std::vector<WorldObject *> wobjects; /* the objects in the world */

    void addObject(WorldObject *obj);
    void addAgent(Agent *agent);
    void loadTrack(const char *file);
    void setRunType(const string str);
    const TrackData_t * getTrack();
    static World &getInstance();

    friend class Graphics;
};

#endif
