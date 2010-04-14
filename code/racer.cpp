/*
 * racer.c
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "world.h"
#include "physics.h"

int main(int argc, char *argv[])
{
    int wres = 800, hres = 640;
    int colorDepth = 32;
    SDL_Surface *screen;
    int done = 0;

    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_SetVideoMode(wres, hres, colorDepth, SDL_OPENGL|SDL_RESIZABLE);

    if (!screen) {
        fprintf(stderr, "Failed to set video mode resolution to %i by %i: %s\n", wres, hres, SDL_GetError());
        SDL_Quit();
        exit(2);
    }

    SDL_WM_SetCaption("Gears", "gears");

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 640, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    World world;
    Physics phys = Physics(&world);

    Vec3f_t p = {100, 0, 100};
    Vec3f_t v = {1, 0, 0};
    Agent a = Agent(p);
    CopyV3f(v, a.pos.dir);

    world.agents.push_back(a);

    Kinematic *k = new Kinematic;
    while (!done) {
        glClear(GL_COLOR_BUFFER_BIT);

        for (unsigned int i = 0; i < world.agents.size(); i++)
        {
            world.agents[i].getKinematic(k);

            glBegin(GL_QUADS);
            glColor3f(0,1,0);
            glVertex3f(k->pos[0]-5, k->pos[2]+5, 0);
            glVertex3f(k->pos[0]+5, k->pos[2]+5, 0);
            glVertex3f(k->pos[0]+5, k->pos[2]-5, 0);
            glVertex3f(k->pos[0]-5, k->pos[2]-5, 0);
            glEnd();
        }

        phys.simulate(0.001);


        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    done = 1;
                    break;
            }
        }
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}
