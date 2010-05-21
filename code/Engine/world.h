#ifndef WORLD_H
#define WORLD_H

#include "allclasses.h"
#include "Utilities/vec3f.h"
#include "Utilities/matrix.h"
#include "Utilities/quat.h"
#include "Graphics/camera.h"
#include "Physics/physics.h"
extern "C" {
#include "Parser/track-parser.h"
}
#include <GL/gl.h>
#include <ode/ode.h>
#include <vector>
#include <string>
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
    PlayerController *player; // for server
    WorldObject(PGeom * pobject, GObject * gobject, SObject * sobject, Agent * agent);

    Vec3f getPos();
    void setPos(Vec3f position);
    void setQuat(Quatf_t quat);
    void getRot(Mat4x4f_t);
    void getQuat(Quatf_t);

    void draw();
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
    static float xMax;   /* XXX what are these? */
    static float zMax;
    dWorldID ode_world;
    dSpaceID ode_space;
    std::vector<WorldObject *> wobjects; /* the objects in the world */
    std::vector<Light *> lights; /* !< the lights in the world */
    std::vector<Widget *> widgets; /* !< the widgets in the world */

    string assetsDir;   /* !< base directory for asset files */

    void addObject(WorldObject *obj);

    int numAgents();

    void addAgent(Agent *agent);
    void addLight(Light *light);
    void addWidget(Widget *widget);
    Agent *placeAgent(int place);/*!<return a placed agent not yet in wobjects*/
    void makeAI();
    void makePlayer();
    void makeAgents();
    void makeSkybox();
    void loadTrack(const char *file);
    void setRunType(const string str);
    const TrackData_t * getTrack();
    static World &getInstance();
    void setDir(const string);

    friend class Graphics;
};

std::ostream &operator<<(std::ostream &, const WorldObject &);

#endif
