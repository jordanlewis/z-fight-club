#include "graphics.h"
#include "Engine/world.h"
#include "Physics/physics.h"
#include "Utilities/vector.h"
#include "Utilities/error.h"
#include "Agents/ai.h"
#include "hud.h"
#include <SDL/SDL.h>

#define MAX_TRAIL_LENGTH 3600 

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

Graphics Graphics::_instance;

Graphics::Graphics()
{
    initialized = false;
}

Graphics::~Graphics()
{
}

void Graphics::initGraphics()
{
    /* set up SDL */
    int wres = 800, hres = 640;
    int colorDepth = 32;
    SDL_Surface *screen;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    screen = SDL_SetVideoMode(wres, hres, colorDepth, SDL_OPENGL|SDL_RESIZABLE);

    if (!screen) {
        fprintf(stderr, "Failed to set video mode resolution to %i by %i: %s\n", wres, hres, SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_WM_SetCaption("z fight club presents: Tensor Rundown", "Tensor Rundown");
    initialized = true;

}

void Graphics::DrawArrow(Vec3f pos, Vec3f dir)
{
    if (!initialized)
	; /* error */

    float l = dir.length();
    /* the 6 verts we need for the arrow */
    std::vector<Vec3f> verts;
    verts.push_back(pos);
    verts.push_back(pos + dir);

    /* make perpendicular vectors */
    Vec3f p1 = dir.perp();
    Vec3f p2 = dir.perp(p1);

    verts.push_back(pos + (dir * 0.7f) + (p1 * l * 0.3f));
    verts.push_back(pos + (dir * 0.7f) - (p1 * l * 0.3f));
    verts.push_back(pos + (dir * 0.7f) + (p2 * l * 0.3f));
    verts.push_back(pos + (dir * 0.7f) - (p2 * l * 0.3f));

    /* vertex array in opengl usable form */
    float *rawVerts = makeArray(verts);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, rawVerts);
    uint16_t lineIndices[2 * 5] = {0, 1, 1, 2, 1, 3, 1, 4, 1, 5}; /* 2 indices per line, 5 lines */
    glDrawElements(GL_LINES, 2 * 5, GL_UNSIGNED_SHORT, lineIndices);

    glDisableClientState(GL_VERTEX_ARRAY);

    delete [] rawVerts;
}

void Graphics::render()
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    World *world = &World::getInstance();

    GLfloat light_position[]={ 10.0, 10.0, -10.0, 1.0 };
    GLfloat light_color[]={ 1.0, 1.0, 1.0, 1.0 };
    GLfloat ambient_color[]={ 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_specular[]={ .2, .2, .2, 1.0 };

    glPushMatrix();
    world->camera.setProjectionMatrix();

    /* render 3d graphics */

    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
    glLightfv(GL_LIGHT0, GL_POSITION, light_position );
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color );
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color );
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color );
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glClearColor(.2f, .2, .8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_LIGHTING);

    // A floor is useful, but this should be cleaned up soon
    glBegin(GL_QUADS);
        glColor3f(0.4,0.3,0.4);
      glVertex3f(-5,0,-5);
      glVertex3f(-5,0,5);
        glColor3f(0.2,0.4,0.4);
      glVertex3f(5,0,5);
      glVertex3f(5,0,-5);
    glEnd();

    glEnable(GL_LIGHTING);
    for (vector<WorldObject *>::iterator i = world->wobjects.begin();
	 i != world->wobjects.end(); i++)
    {
        (*i)->draw();
    }

    render(world->track);

    for(vector<Agent*>::iterator i = world->agents.begin(); i != world->agents.end(); i++) {
	    render(*i);
    }

    AIManager &aiManager = AIManager::getInstance();

    for (vector<AIController *>::iterator i = aiManager.controllers.begin(); i != aiManager.controllers.end(); i++) {
	render(*i);
    }
    
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(2.0f, 0.0f, 0.0f));
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 2.0f, 0.0f));
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 2.0f));
    SDL_GL_SwapBuffers();
    glPopMatrix();

    glPushMatrix();
    world->camera.setOrthoMatrix();

    /* render 2d stuff */
		Hud hud = Hud::getInstance();
		render(&hud);

    glPopMatrix();
}

void Graphics::render(Agent * agent)
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    agent->trail.push_back(agent->kinematic.pos);

    if (agent->trail.size() > MAX_TRAIL_LENGTH)
	agent->trail.erase(agent->trail.begin());

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(agent->kinematic.pos.x, agent->kinematic.pos.y, agent->kinematic.pos.z);
    glColor3f(1,1,1);
    DrawArrow(Vec3f(0.0, 0.0, 0.0), agent->kinematic.vel);
    DrawArrow(Vec3f(0.0, 0.0, 0.0), agent->kinematic.orientation_v);

    glPopMatrix();
    glColor3f(1,0,0);
    render(agent->trail);
}

void Graphics::render(AIController *aiController)
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    glColor3f(0,1,0);
    DrawArrow(aiController->agent->getKinematic().pos,
              aiController->path.knots.front());
    render(aiController->path.knots);
}

void Graphics::render(TrackData_t *track)
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    if (track) {
	int i, j;
	/* load in the vertices */
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, track->verts);

	/* draw sectors */
	for (i = 0; i < track->nSects; i++) {
	    uint16_t *lineIndices = new uint16_t[track->sects[i].nEdges];
	    for (j = 0; j < track->sects[i].nEdges; j++) {
		lineIndices[j] = track->sects[i].edges[j].start;
	    }
	    glDrawElements(GL_LINE_LOOP, track->sects[i].nEdges, GL_UNSIGNED_SHORT, lineIndices);
	    delete [] lineIndices;
	}
	glDisableClientState(GL_VERTEX_ARRAY);

        glColor3f(1,1,0);
        GLUquadricObj *quadobj = gluNewQuadric();
        Vec3f_t v;
        Segment_t *seg;
        int len;
        float ang;

        glBegin(GL_LINES);
        for (i = 0; i < track->nLanes; i++)
        {
            for (j = 0; j < track->lanes[i].nSegs; j++)
            {
                seg = &track->lanes[i].segs[j];
                switch (track->lanes[i].segs[j].kind) {
                    case LINE_SEGMENT:
                        glVertex3fv(track->verts[seg->start]);
                        glVertex3fv(track->verts[seg->end]);
                        break;
                    case ARC_SEGMENT:
                        glEnd();
                        CopyV3f(track->verts[seg->center],v);
                        glPushMatrix();
                        glTranslatef(v[0], v[1], v[2]);
                        glRotatef(-90,1,0,0);
                        SubV3f(v, track->verts[seg->start], v);
                        len = LengthV3f(v);
                        ang = seg->end > seg->start ? seg->angle : -seg->angle;
                        gluPartialDisk(quadobj, len-.05, len, 40, 5,
                                        0, ang);
                        gluPartialDisk(quadobj, len-.05, len, 40, 5,
                                    0, ang);

                        glPopMatrix();
                        glBegin(GL_LINES);
                        break;
                }
            }
        }
        glEnd();
        gluDeleteQuadric(quadobj);

    }
}

void Graphics::render(Hud *hud)
{
		for (vector<Widget>::iterator it = hud->widget.begin(); it != hud->widget.end(); it++) {
				it->draw();
		}
}

void Graphics::render(std::vector<Vec3f> path)
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    unsigned int i;

    float *rawVerts = makeArray(path);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, rawVerts);

    uint16_t *lineIndices = new uint16_t[path.size()];
    for (i = 0; i < path.size(); i++)
	lineIndices[i] = i;

    glDrawElements(GL_LINE_STRIP, path.size(), GL_UNSIGNED_SHORT, lineIndices);

    glDisableClientState(GL_VERTEX_ARRAY);

    delete []rawVerts;
}

void Graphics::render(std::deque<Vec3f> path)
{
    if (!initialized) {
	Error error = Error::getInstance();
	error.log(GRAPHICS, CRITICAL, "Render function called without graphics initialization\n");
	exit(0);
    }

    unsigned int i;
    
    float *rawVerts = makeArray(path);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, rawVerts);

    uint16_t *lineIndices = new uint16_t[path.size()];
    for (i = 0; i < path.size(); i++)
	lineIndices[i] = i;

    glDrawElements(GL_LINE_STRIP, path.size(), GL_UNSIGNED_SHORT, lineIndices);

    glDisableClientState(GL_VERTEX_ARRAY);

    delete []rawVerts;
}

Graphics &Graphics::getInstance()
{
    return _instance;
}

void Graphics::DrawCube(Vec3f center)
{
    glBegin(GL_QUADS);
    Vec3f toDraw;
    float inc[2][4] = {{1.0,0.0,-1.0,0.0},
                       {0.0,1.0,0.0,-1.0}};
    for(int i=0;i<3;i++) {
        int a = (i+1)%3,
            b = (i+2)%3;
        for(int k=1; k >= -1; k -= 2) {
            toDraw = center;
            toDraw[i] += (0.5*k);
            toDraw[a] -= (0.5*k);
            toDraw[b] -= 0.5;
            for(int j=0; j<4; j++) {
                toDraw[a] += k*inc[0][j];
                toDraw[b] += inc[1][j];
                glVertex3f(toDraw.x,toDraw.y,toDraw.z);
            }
        }
    }
    glEnd();
}
