/*
 * racer.c
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>

int main(int argc, char *argv[])
{
    int wres = 800, hres = 640;
    int colorDepth = 32;
    SDL_Surface *screen;
    int done;

    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_SetVideoMode(wres, hres, colorDepth, SDL_OPENGL|SDL_RESIZABLE);

    if (!screen) {
	fprintf(stderr, "Failed to set video mode resolution to %i by %i: %s\n", wres, hres, SDL_GetError());
	SDL_Quit();
	exit(2);
    }

    SDL_WM_SetCaption("Gears", "gears");

    done = 0;
    while (!done) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
	    switch(event.type) {
		case SDL_QUIT:
		    done = 1;
		    break;
	    }
	}
    }
    SDL_Quit();
    return 0;
}
