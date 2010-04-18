#include "graphics.h"
#include "../Engine/world.h"
#include <SDL/SDL.h>

#if defined(__APPLE__) && defined(__MACH__)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

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
}

void Graphics::render(World * world)
{
    world->camera.setProjectionMatrix();
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glClearColor(.2f, .2, .8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int i;
    for (i = 0; i < world->agents.size(); i++) {
	render(&(world->agents[i]));
    }
    SDL_GL_SwapBuffers();
}

void Graphics::render(Agent * agent)
{
    if (initialized) {
	glPushMatrix();
	glTranslatef(agent->kinematic.pos.x, agent->kinematic.pos.y, agent->kinematic.pos.z);
	gluSphere(NULL, 1.0, 18, 12);
	glPopMatrix();
    } /* else */
	/* error */
}
