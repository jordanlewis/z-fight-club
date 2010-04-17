/*! \file util.c
 *
 * \brief This file contains various utility functions.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "defs.h"
#include <sys/time.h>
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif
#include <stdio.h>

/* GetTime:
 */
double GetTime ()
{
    struct timeval	t;
    
    gettimeofday (&t, 0);
    return (double)t.tv_sec + 0.000001*(double)t.tv_usec;

}

/* CheckMalloc:
 */
void *CheckMalloc (size_t nbytes)
{
    void *obj = malloc(nbytes);
    if (obj == 0) {
	fprintf(stderr, "Fatel error: unable to allocate %d bytes\n", (int)nbytes);
	exit (1);
    }

    return obj;
}

#ifndef NDEBUG
/* CheckError:
 */
/* void CheckError (const char *file, int lnum)
{
    GLenum err = glGetError ();
    if (err != GL_NO_ERROR) {
	fprintf (stderr, "Error [%s:%d]: %s\n", file, lnum, gluErrorString(err));
    }
}*/
#endif

#ifdef __cplusplus
}
#endif
