#include "graphics.h"
#include "Engine/world.h"
#include "Physics/physics.h"
#include <SDL/SDL.h>

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
	exit(2);
    }

    SDL_WM_SetCaption("Racer", "racer");
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
    World *world = &World::getInstance();

    GLfloat light_position[]={ 10.0, 10.0, -10.0, 1.0 };
    GLfloat light_color[]={ 1.0, 1.0, 1.0, 1.0 };
    GLfloat ambient_color[]={ 0.2, 0.2, 0.2, 1.0 };
    GLfloat mat_specular[]={ 1.0, 1.0, 1.0, 1.0 };

    world->camera.setProjectionMatrix();

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
    Graphics::renderBoxPGeoms();

    render(world->track);

    // We should figure out how to use iterators and use one here
    unsigned int i;
    for (i = 0; i < world->agents.size(); i++) {
	    render(world->agents[i]);
    }
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(2.0f, 0.0f, 0.0f));
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 2.0f, 0.0f));
    DrawArrow(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 2.0f));
    SDL_GL_SwapBuffers();
}

void Graphics::render(Agent * agent)
{
    if (!initialized)
	; /* error */

    agent->trail.push_back(agent->kinematic.pos);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(agent->kinematic.pos.x, agent->kinematic.pos.y, agent->kinematic.pos.z);
    GLUquadric *quad = gluNewQuadric();
    gluSphere(quad, 0.1, 18, 12);
    DrawArrow(Vec3f(0.0, 0.0, 0.0), agent->kinematic.vel);
    DrawArrow(Vec3f(0.0, 0.0, 0.0), agent->kinematic.orientation_v);

    glPopMatrix();
    render(agent->trail);
}

void Graphics::render(TrackData_t *track)
{
    if (!initialized)
	; /* error */
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
    }
}

void Graphics::render(std::vector<Vec3f> path)
{
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
    Vec3f toDraw = center;
    float inc[2][4] = {{-0.5,0.5,0.5,-0.5},
                       {-0.5,-0.5,0.5,0.5}};
    for(int i=0;i<3;i++) {
        int a = (i+1)%3,
            b = (i+2)%3;
        for(int k=1; k>=-1; k-=2) {
            toDraw[i] += (0.5*k);
            for(int j=0; j<4; j++) {
                toDraw[a] += inc[0][j];
                toDraw[b] += inc[1][j];
                glVertex3f(toDraw.x,toDraw.y,toDraw.z);
                toDraw[a] = center[a];
                toDraw[b] = center[b];
            }
            toDraw[i] = center[i];
        }
    }
    glEnd();
}


void Graphics::renderBoxPGeoms()
{
    /* XXX HACK!!!!!! XXX 
     * I wrote this just so we can temporarily see the sides of the track.
     * It is violating the physics abstraction by peeking at the pgeoms, and
     * also if any of the pgeoms we saved aren't boxes then this will probably
     * blow up.
     * TODO: find some way of representing all static world geometry, and stick
     * it in world so this awful abstraction fail doesn't break us.
     */
    float sides[3];
    GLdouble matrix[16];
    const dReal *pos;
    const dReal *R;
    float quat[4];
    float theta;
    glMatrixMode(GL_MODELVIEW);
    for (unsigned int i = 0; i < Physics::getInstance().pgeoms.size();i++)
    {
        dGeomID box = Physics::getInstance().pgeoms[i]->getGeom();

        dGeomBoxGetLengths(box, sides);
        pos = dGeomGetPosition(box);
        R = dGeomGetRotation(box);
        dGeomGetQuaternion(box, quat);
        theta = 2 * acos(quat[0]);
        matrix[0]=R[0];
        matrix[1]=R[4];
        matrix[2]=R[8];
        matrix[3]=0;
        matrix[4]=R[1];
        matrix[5]=R[5];
        matrix[6]=R[9];
        matrix[7]=0;
        matrix[8]=R[2];
        matrix[9]=R[6];
        matrix[10]=R[10];
        matrix[11]=0;
        matrix[12]=pos[0];
        matrix[13]=pos[1];
        matrix[14]=pos[2];
        matrix[15]=1;
        glPushMatrix();
        glMultMatrixd (matrix);

        float lx = sides[0]*0.5f;
        float ly = sides[1]*0.5f;
        float lz = sides[2]*0.5f;

        // sides
        glBegin (GL_TRIANGLE_STRIP);
        glNormal3f (-1,0,0);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,-ly,lz);
        glVertex3f (-lx,ly,-lz);
        glVertex3f (-lx,ly,lz);
        glNormal3f (0,1,0);
        glVertex3f (lx,ly,-lz);
        glVertex3f (lx,ly,lz);
        glNormal3f (1,0,0);
        glVertex3f (lx,-ly,-lz);
        glVertex3f (lx,-ly,lz);
        glNormal3f (0,-1,0);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,-ly,lz);
        glEnd();

        // top face
        glBegin (GL_TRIANGLE_FAN);
        glNormal3f (0,0,1);
        glVertex3f (-lx,-ly,lz);
        glVertex3f (lx,-ly,lz);
        glVertex3f (lx,ly,lz);
        glVertex3f (-lx,ly,lz);
        glEnd();

        // bottom face
        glBegin (GL_TRIANGLE_FAN);
        glNormal3f (0,0,-1);
        glVertex3f (-lx,-ly,-lz);
        glVertex3f (-lx,ly,-lz);
        glVertex3f (lx,ly,-lz);
        glVertex3f (lx,-ly,-lz);
        glEnd();
        glPopMatrix();
    }


}

