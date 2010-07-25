#ifndef WORLD_H
#define WORLD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include "Utilities/matrix.h"
#include "Utilities/quat.h"
#include "Utilities/path.h"
#include "Graphics/camera.h"
#include "Graphics/hud.h"
#include "Graphics/graphics.h"
#include "Physics/physics.h"
#include "Network/network.h"
#include <ode/ode.h>
#include <vector>
#include <string>
extern "C" {
#include "Parser/track-parser.h"
}

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
using namespace std;

/*! Collects all of the possible representations of an object
 * in the world.
 *
 * It isn't necessary for a WorldObject to have all of these representations.
 * a NULL pointer means "no representation".
 * i.e. Only an agent needs a valid agent; objects that do not make sound
 * have NULL sound pointers, etc.
 */
class WorldObject
{
  private:
    Vec3f pos; /*!< Tracks the position of the object in the game world */
    Quatf_t quat; /*!< A Quaternion tracking the orientation of our object */
  public:
    PGeom *pobject; /*!< Tracks all physical properties of our object */
    GObject *gobject; /*!< Tracks all graphical properties of our object */
    SObject *sobject; /*!< Tracks all sound properties of our object */
    Agent *agent; /*!< Tracks properties related to our object as an agent */
    WorldObject *parent;
    int parent_index;  /*!< the index of our object in the parent's array*/
    netObjID_t netID; /*!< A network-synched ID for this object */

    vector<WorldObject *> children;
    PlayerController *player; // for server

    bool alive; /*!< Does this object still have time to live? */
    double timeStarted; /*!< The creation time of an object */
    double ttl; /*!< A time to live for this object.  ttl has a positive
                 * value for temporary objects, -1 for permanent objects. 
                 */

    WorldObject(PGeom * pobject, GObject * gobject, SObject * sobject,
                Agent * agent, double ttl=-1);
    ~WorldObject();

    void clear(); /*!< Destroys this WorldObject and deletes its associated
                  * pobject, gobject, etc representations. */

    virtual Vec3f getPos();
    void setPos(Vec3f position);
    void setQuat(Quatf_t quat);
    void getRot(Mat4x4f_t);
    void getQuat(Quatf_t);
    void addChild(WorldObject *);
    void deleteChild(int i);

    void draw(Layer_t); /*!< If this object has an associated gobject, draw 
                         * calls the gobject's draw method. */
};

class CameraFollower : public WorldObject
{
  public:
    CameraFollower(PGeom * pobject, GObject * gobject, SObject * sobject,
                   Agent * agent, Camera *camera);
    virtual Vec3f getPos();
  private:
    Camera *camera;
    CameraFollower(); // don't create a follower without a camera, and don't copy it
};

class ParticleStreamObject : public WorldObject
{
  public:
    GParticleObject *gobject;
    ParticleStreamObject(PGeom *pobject, GParticleObject *gobject,
                         SObject *sobject, Agent * agent, double ttl=-1);
};

/* \brief Light representation for a light in a scene
 */
class Light
{
  public:
      GLfloat pos[4];
      GLfloat lightColor[4];
      GLfloat ambientColor[4];
      GLfloat specColor[4];
      Light();
      Light(GLfloat *, GLfloat *, GLfloat *, GLfloat *);
      ~Light();
      void setup(GLenum); /* makes opengl calls to add the light */
};

//What mode are we running our game in?
typedef enum {
    SOLO = 0,
    CLIENT = 1,
    SERVER = 2
} RunType_t;

/*! Originally a singleton representing the state of the game world, now
 * contains a hodgepodge of global information.
 *
 */
class World
{
  private:
    // Prevent construction, destruction, copying, or assignment in singleton
    World();
    ~World();
    World(const World&);
    World& operator=(const World&);

    static World _instance;
    Error *error;
  public:
    int PlayerQty;
    int playerSkin; /*!< which mesh the player wants to use.  Transferred to
                     * the player's agent once it is created.*/
    int AIQty;
    bool nox; /*!< Don't run graphics (no x server available). */
    bool nosound;
    bool nomusic;
    bool fullscreen;
    int wres; /*!< Resolution width (used for fullscreen resolution only). */
    int hres; /*!< Resolution height (used for fullscreen resolution only). */
    RunType_t runType; /*!< Are we running in solo, client, or server mode? */
    Camera camera;
    TrackData_t *track;
    Path path; /*!< The path to be followed by the AI */
    WorldObject *floorObj;  /*!< The bottom of the track */
    WorldObject *botPlaneObj; /*!< If a player intersects with this plane, they
                               * are reset onto the track.  Occurs if a player
                               * goes well off course, gets shot off of the 
                               * track, etc. */
    dWorldID ode_world; /*!< The physics engine's representation of the world */
    dSpaceID ode_space; /*!< The physics engine's representation of the world's
                         * collision geometry. */
    unsigned int nLaps; /*!< Number of laps to be played. */

    Agent *winner;

    std::vector<WorldObject *> wobjects; /*!< the objects in the world */

    /*! the particle systems in the world.  These need to be rendered after
     * everything else to avoid transparency bugs */
    std::vector<ParticleStreamObject*> particleSystems;

    std::vector<Light *> lights; /*!< the lights in the world */
    std::vector<Widget *> widgets; /*!< the widgets in the world */
    SubMenu *pauseMenu;                    /*!< menu to render when the game 
                                            * is paused */
    SubMenu *setupMenu;                    /*!< menu for setting up the game */

    string assetsDir;   /*!< base directory for asset files */

    void addObject(WorldObject *obj); /*!< add an existing WorldObject
                                       * into the wobjects vector. */
    void addObject(ParticleStreamObject *obj); /*!< add an existing particle
                                                * stream into the wobjects 
                                                * vector */
    void cleanObjects(); /*!< deletes unneeded wobjects from the wobject list */

    int numAgents(); /*!< Count the number of agents in the wobjects array */

    /*! Does all initialization associated with creating an agent. */
    void addAgent(Agent *agent, int skin = -1);
    void addLight(Light *light);
    void addWidget(Widget *widget);
    Agent *placeAgent(int place);/*!<return a placed agent not yet in wobjects*/
    Agent *makeCar(); /*!< Create a car unattached to any control structures */
    Agent *makeAI(); /*!< Create an AI player's car */
    Agent *makePlayer(); /*!< Create a human player's car */
    void makeAgents(); /*!< Create all agents to be placed in the game */
    void makeSkybox();
    void loadTrack(const char *file);
    void setRunType(const string str);
    const TrackData_t * getTrack();
    static World &getInstance(); /*!< Grab the global world instance */
    void setDir(const string); /*!< set the assets directory */

    friend class Graphics;
};

void addAI(); /*!< Adds an AI player to the world */

std::ostream &operator<<(std::ostream &, const WorldObject &);

#endif
