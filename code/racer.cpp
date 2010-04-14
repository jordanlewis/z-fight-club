/*
 * racer.c
 *
 * main racer file
 */

#include <iostream>
#include <SDL/SDL.h>
#include "scheduler.h"

int main(int argc, char *argv[])
{
    Scheduler scheduler;
    
    World world;

    scheduler.loopForever(&world);
}
