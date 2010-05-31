#include "graphics.h"
#include "Engine/world.h"
#include "Physics/physics.h"
#include "Utilities/vector.h"
#include "Utilities/error.h"
#include "Engine/scheduler.h"
#include "Agents/ai.h"
#include "Agents/agent.h"
#include "hud.h"
#include <SDL/SDL.h>

#define MAX_TRAIL_LENGTH 3600

extern "C" {
#include "Parser/track-parser.h"
}

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

Graphics Graphics::_instance;

Graphics::Graphics() :
    Component(),
    world(&World::getInstance()),
    error(&Error::getInstance())
{
    frequency = 60;
    initialized = false;
}

Graphics::~Graphics()
{
}

void Graphics::initGraphics()
{
    if (initialized == true)
        return ;
    /* set up SDL */
    int wres = world->camera.wres, hres = world->camera.hres;
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

    if (world->runType == SERVER)
    {
        SDL_WM_SetCaption("z fight club presents: Tensor Rundown (server)", "Tensor Rundown");
    }
    else
    {
        SDL_WM_SetCaption("z fight club presents: Tensor Rundown", "Tensor Rundown");
    }
    int argc = 0;
    glutInit(&argc, NULL);
    initialized = true;

}

void Graphics::DrawArrow(Vec3f pos, Vec3f dir)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    glDisable(GL_LIGHTING);
    float l = dir.length();
    /* the 6 verts we need for the arrow */
    Vec3f_t verts[6];
    pos.toArray(        verts[0]);
    (pos + dir).toArray(verts[1]);

    /* make perpendicular vectors */
    Vec3f p1 = dir.perp();
    Vec3f p2 = dir.perp(p1);

    (pos + (dir * 0.7f) + (p1 * l * 0.3f)).toArray(verts[2]);
    (pos + (dir * 0.7f) - (p1 * l * 0.3f)).toArray(verts[3]);
    (pos + (dir * 0.7f) + (p2 * l * 0.3f)).toArray(verts[4]);
    (pos + (dir * 0.7f) - (p2 * l * 0.3f)).toArray(verts[5]);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verts);
    uint16_t lineIndices[2 * 5] = {0, 1, 1, 2, 1, 3, 1, 4, 1, 5}; /* 2 indices per line, 5 lines */
    glDrawElements(GL_LINES, 2 * 5, GL_UNSIGNED_SHORT, lineIndices);

    glDisableClientState(GL_VERTEX_ARRAY);

    glEnable(GL_LIGHTING);
}

void Graphics::render()
{
    if (!start())
        return;

    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    RaceState_t state = Scheduler::getInstance().raceState;

    world->camera.setProjectionMatrix();

    /* render 3d graphics */

    GLfloat mat_specular[]={ .2, .2, .2, 1.0 };
    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);

    /* fog */
    /* GLfloat fog_color[] = {.5, .2, .2, 1.0};
    glEnable(GL_FOG);
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END, 100.0f);
    glFogi(GL_FOG_MODE, GL_LINEAR); */

    if(world->lights.size() > 0) {
        for (vector<Light *>::iterator i = world->lights.begin(); i != world->lights.end() && world->lights.begin() - i < GL_MAX_LIGHTS; i++)
        {

        }
    } else {
        /* safety net to make sure even if there are no lights
         * something still shows up on the screen */
        GLfloat light_position[]={ 10.0, 10.0, -10.0, 1.0 };
        GLfloat light_color[]={ 1.0, 1.0, 1.0, 1.0 };
        GLfloat ambient_color[]={ 0.2, 0.2, 0.2, 1.0 };

        glLightfv(GL_LIGHT0, GL_POSITION, light_position );
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color );
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_color );
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color );
        glEnable(GL_LIGHT0);
    }


    glClearColor(.1f, .1, .1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.2,0.4,0.4);
    glEnable(GL_LIGHTING);

    if (state != SETUP)
    {
        /* Draw regular things */
        for (vector<WorldObject *>::iterator i = world->wobjects.begin(); i != world->wobjects.end(); i++)
        {
            (*i)->draw();
        }

        for (vector<ParticleStreamObject *>::iterator i = world->particleSystems.begin();
            i != world->particleSystems.end(); i++)
        {
            (*i)->draw();
        }

        // Uncomment for AI debug rendering
        /*
        AIManager &ai = AIManager::getInstance();
        for (vector<AIController *>::iterator i = ai.controllers.begin();
            i != ai.controllers.end(); i++)
        {
                render(*i);
        }
        */

        render(world->track);



    }

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    world->camera.setOrthoMatrix();
    /* draw the widgets */

    if (state == SETUP)
        world->setupMenu->draw();
    else
    {
        for (vector<Widget *>::iterator i = world->widgets.begin(); i != world->widgets.end(); i++)
        {
            (*i)->draw();
        }
        if (state == PAUSE)
            world->pauseMenu->draw();
    }

    glPopMatrix();

    SDL_GL_SwapBuffers();

    finish();
}

void Graphics::render(Agent * agent)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    agent->trail.push_back(agent->kinematic.pos);

    if (agent->trail.size() > MAX_TRAIL_LENGTH)
        agent->trail.erase(agent->trail.begin());


    glColor3f(1,1,1);
    DrawArrow(Vec3f(0.0, 0.0, 0.0), agent->kinematic.orientation_v);

    glColor3f(0,0,0);
    render(agent->trail);
}

void Graphics::render(AIController *aiController)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    glColor3f(0,1,0);

    // Uncomment to render AI debug info
    Kinematic k = aiController->agent->getKinematic();
    render(aiController->path.knots);

    Vec3f closest = aiController->path.closestPoint(aiController->target);
    glColor3f(0,1,0);
    DrawArrow(k.pos, aiController->target - k.pos);
    DrawArrow(aiController->target, closest - aiController->target);
    if (aiController->seeObstacle)
    {
        glColor3f(1,0,0);
        DrawArrow(k.pos, aiController->antiTarget - k.pos);
    }


}

void Graphics::render(TrackData_t *track)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    if (track) {
        int i, j;
        /* load in the vertices */

        Vec3f_t vert;

        GLUquadricObj *quadobj = gluNewQuadric();
        Vec3f_t v;
        Segment_t *seg;
        int len;
        float ang;

        glColor3f(1,1,0);
        glBegin(GL_LINES);
        for (i = 0; i < track->nLanes; i++)
        {
            for (j = 0; j < track->lanes[i].nSegs; j++)
            {
                seg = &track->lanes[i].segs[j];
                switch (track->lanes[i].segs[j].kind) {
                    case LINE_SEGMENT:
                        CopyV3f(track->verts[seg->start], vert);
                        glVertex3f(vert[0], vert[1] + .01, vert[2]);
                        CopyV3f(track->verts[seg->end], vert);
                        glVertex3f(vert[0], vert[1] + .01, vert[2]);
                        break;
                    case ARC_SEGMENT:
                        glEnd();
                        CopyV3f(track->verts[seg->center],v);
                        glPushMatrix();
                        glTranslatef(v[0], v[1] + .01, v[2]);
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

        /* draw finish line */
        glDisable(GL_LIGHTING);
        glColor3f(1,0,1);
        glLineStipple(1, 0x0C0F);
        glLineWidth(4);
        glBegin(GL_LINES);
        glVertex3fv(track->verts[track->sects[0].edges[0].start]);
        glVertex3fv(track->verts[track->sects[0].edges[1].start]);
        glEnd();
        glLineWidth(1);
        glDisable(GL_LINE_STIPPLE);
        glEnable(GL_LIGHTING);

    }
}

void Graphics::render(std::vector<Vec3f> path)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    unsigned int i;

    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < path.size(); i++)
    {
        glVertex3f(path[i].x, path[i].y, path[i].z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void Graphics::render(std::deque<Vec3f> path)
{
    if (!initialized) {
        error->log(GRAPHICS, IMPORTANT, "Render function called without graphics initialization\n");
        return;
    }

    unsigned int i;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < path.size(); i++)
    {
        glVertex3f(path[i].x, path[i].y, path[i].z);
    }
    glEnd();
    glEnable(GL_LIGHTING);

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
