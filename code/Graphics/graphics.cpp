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
}

Graphics::~Graphics()
{
}

void Graphics::InitGraphics()
{
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

void Graphics::render(const World * const world)
{
}
