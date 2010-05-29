#ifndef WORLD_H
#define WORLD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include "Utilities/matrix.h"
#include "Utilities/quat.h"
#include "Utilities/path.h"
#include "Graphics/camera.h"
#include "Graphics/hud.h"
#include "Physics/physics.h"
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
    Quatf_t quat;
  public:
    PGeom *pobject;
    GObject *gobject;
    SObject *sobject;
    Agent *agent;
    WorldObject *parent;
    int parent_index;           /* !< the index we have in our parent's array */
    vector<WorldObject *> children;
    PlayerController *player; // for server

    bool alive;
    double timeStarted;
    double ttl;

    WorldObject(PGeom * pobject, GObject * gobject, SObject * sobject,
                Agent * agent, double ttl=-1);
    ~WorldObject();

    void clear();

    virtual Vec3f getPos();
    void setPos(Vec3f position);
    void setQuat(Quatf_t quat);
    void getRot(Mat4x4f_t);
    void getQuat(Quatf_t);
    void addChild(WorldObject *);
    void deleteChild(int i);

    void draw();
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
    int AIQty;
    bool nox;
    bool nosound;
    RunType_t runType;
    Camera camera;
    TrackData_t *track;
    Path path;
    WorldObject *floorObj;
    WorldObject *botPlaneObj;
    dWorldID ode_world;
    dSpaceID ode_space;

    std::vector<WorldObject *> wobjects; /* the objects in the world */
    /* the particle systems in the world -- these need to be rendered after everything else
       to avoid transparency bugs */
    std::vector<ParticleStreamObject*> particleSystems;

    std::vector<Light *> lights; /* !< the lights in the world */
    std::vector<Widget *> widgets; /* !< the widgets in the world */
    SubMenu *pauseMenu;                    /* !< menu to render when the game is paused */
    SubMenu *setupMenu;                    /* !< menu for setting up the game */

    string assetsDir;   /* !< base directory for asset files */

    void addObject(WorldObject *obj);
    void addObject(ParticleStreamObject *obj);
    void cleanObjects();

    int numAgents();

    void addAgent(Agent *agent);
    void addLight(Light *light);
    void addWidget(Widget *widget);
    Agent *placeAgent(int place);/*!<return a placed agent not yet in wobjects*/
    Agent *makeCar(); //Create a car unattached to any control structures
    Agent *makeAI();
    Agent *makePlayer();
    void makeAgents();
    void makeSkybox();
    void loadTrack(const char *file);
    void setRunType(const string str);
    const TrackData_t * getTrack();
    static World &getInstance();
    void setDir(const string);

    friend class Graphics;
};

void addAI();

std::ostream &operator<<(std::ostream &, const WorldObject &);

#endif
