#include "graphics.h"
#include "Engine/world.h"
#include <SDL/SDL.h>

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

void Graphics::render(World * world)
{
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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glClearColor(.2f, .2, .8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int i;
    for (i = 0; i < world->agents.size(); i++) {
	render(world->agents[i]);
    }
    SDL_GL_SwapBuffers();
}

void Graphics::render(Agent * agent)
{
    if (initialized) {
    	glPushMatrix();
    	glTranslatef(agent->kinematic.pos.x, agent->kinematic.pos.y, agent->kinematic.pos.z);
    	GLUquadric *quad = gluNewQuadric();
    	gluSphere(quad, 1.0, 18, 12);
    	glPopMatrix();
    } /* else */
	/* error */
}

Graphics &Graphics::getInstance()
{
    return _instance;
}
