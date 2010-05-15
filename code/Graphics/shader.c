/*! \file shader.c
 *
 * \brief This file implements an API for loading and using shaders.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include "Utilities/defs.h"
#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#endif
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "shader.h"

#ifndef GL_VERSION_2_0
#  error need OpenGL 2.0 support
#endif

#include <errno.h>

struct struct_vshader {
    GLuint	id;
};

struct struct_pshader {
    GLuint	id;
};

struct struct_program {
    GLuint	id;		//!< the ID of the program object
    GLuint	vshId;		//!< the ID of the vertex shader (0 if no shader)
    GLuint	fshId;		//!< the ID of the fragment shader (0 if no shader)
};

static GLchar *ReadFile (const char *file)
{
    struct stat	st;

  /* get the size of the file */
    if (stat(file, &st) < 0) {
	fprintf (stderr, "error reading \"%s\": %s\n", file, strerror(errno));
	return 0;
    }
    off_t sz = st.st_size;

  /* open the file */
    FILE *strm = fopen(file, "r");
    if (strm == NULL) {
	fprintf (stderr, "error opening \"%s\": %s\n", file, strerror(errno));
	return 0;
    }

  /* allocate the buffer */
    GLchar *buf = NEWVEC(GLchar, sz+1);

  /* read the file */
    if (fread(buf, 1, sz, strm) != sz) {
	fprintf (stderr, "error reading \"%s\": %s\n", file, strerror(errno));
	free (buf);
    }
    buf[sz] = '\0';

    fclose (strm);

    return buf;

}

/* LoadShader:
 */
static bool LoadShader (const char *file, GLuint shaderId)
{
    GLint sts;

    GLchar *src = ReadFile (file);

    if (src == 0)
	return false;

    glShaderSource (shaderId, 1, (const GLchar **)&src, 0);
    free (src);

    glCompileShader (shaderId);

  /* check for errors */
    glGetShaderiv (shaderId, GL_COMPILE_STATUS, &sts);
    if (sts != GL_TRUE) {
      /* the compile failed, so report an error */
	glGetShaderiv (shaderId, GL_INFO_LOG_LENGTH, &sts);
	if (sts != 0) {
	    GLchar *log = NEWVEC(GLchar, sts);
	    glGetShaderInfoLog (shaderId, sts, 0, log);
	    fprintf (stderr, "Error compiling shader \"%s\":\n%s\n", file, log);
	    free (log);
	}
	else
	    fprintf (stderr, "Error compiling shader \"%s\".\n", file);
	glDeleteShader (shaderId);
	return false;
    }

    return true;

}

/* LoadVertexShader:
 */
VertexShader_t *LoadVertexShader (const char *file)
{
    GLuint id = glCreateShader (GL_VERTEX_SHADER);

    if (LoadShader (file, id)) {
	VertexShader_t *shader = NEW(VertexShader_t);
	shader->id = id;
    
	return shader;
    }
    else
	return 0;

} /* end of LoadVertexShader */

/* FreeVertexShader:
 */
void FreeVertexShader (VertexShader_t *vsh)
{
    if (vsh != 0) {
	glDeleteShader (vsh->id);
	free (vsh);
    }
}

/* LoadFragmentShader:
 */
FragmentShader_t *LoadFragmentShader (const char *file)
{
    GLuint id = glCreateShader (GL_FRAGMENT_SHADER);

    if (LoadShader (file, id)) {
	FragmentShader_t *shader = NEW(FragmentShader_t);
	shader->id = id;

	return shader;
    }
    else
	return 0;

} /* end of LoadFragmentShader */

/* FreeFragmentShader:
 */
void FreeFragmentShader (FragmentShader_t *fsh)
{
    if (fsh != 0) {
	glDeleteShader (fsh->id);
	free (fsh);
    }
}

/* CreateShaderProgram:
 */
ShaderProgram_t *CreateShaderProgram (VertexShader_t *vsh, FragmentShader_t *fsh)
{
    GLuint	progId, vshId, fshId;
    GLint	sts;

    progId = glCreateProgram ();
    if (vsh != 0) {
	vshId = vsh->id;
	glAttachShader (progId, vshId);
    }
    else
	vshId = 0;

    if (fsh != 0) {
	fshId = fsh->id;
	glAttachShader (progId, fshId);
    }
    else
	fshId = 0;

    glLinkProgram (progId);

  /* check for errors in the linking */
    glGetProgramiv (progId, GL_LINK_STATUS, &sts);
    if (sts != GL_TRUE) {
      /* the link failed, so report an error */
	glGetProgramiv (progId, GL_INFO_LOG_LENGTH, &sts);
	if (sts != 0) {
	    GLchar *log = NEWVEC(GLchar, sts);
	    glGetProgramInfoLog (progId, sts, 0, log);
	    fprintf (stderr, "Error linking program:\n%s\n", log);
	    free (log);
	}
	else
	    fprintf (stderr, "Error linking program.\n");
	glDeleteProgram (progId);
	return 0;
    }

    ShaderProgram_t *prog = NEW(ShaderProgram_t);
    prog->id = progId;
    prog->vshId = vshId;
    prog->fshId = fshId;

    return prog;

}

/* FreeShaderProgram:
 */
void FreeShaderProgram (ShaderProgram_t *prog)
{
    if (prog != 0) {
	if (prog->vshId != 0)
	    glDetachShader (prog->id, prog->vshId);
	if (prog->fshId != 0)
	    glDetachShader (prog->id, prog->fshId);
	glDeleteProgram (prog->id);
	free (prog);
    }
}

/* UseProgram:
 */
void UseProgram (ShaderProgram_t *prog)
{
    if (prog != 0)
	glUseProgram (prog->id);
    else
	glUseProgram (0);
}

/* AttributeLocation:
 */
GLint AttributeLocation (ShaderProgram_t *prog, const char *name)
{
    GLint id = glGetAttribLocation (prog->id, name);
    if (id < 0) {
	fprintf (stderr, "attribute %s is invalid\n", name);
	exit (1);
    }
    return id;
}

/* UniformLocation:
 */
GLint UniformLocation (ShaderProgram_t *prog, const char *name)
{
    GLuint id = glGetUniformLocation (prog->id, name);
    if (id < 0) {
	fprintf (stderr, "uniform %s is invalid\n", name);
	exit (1);
    }
    return id;
}
