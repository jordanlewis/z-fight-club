/*! \file shader.h
 *
 * \brief This file defines an API for loading and using shaders.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _SHADER_H_
#define _SHADER_H_

#include "Utilities/defs.h"

/***** Support for loading shaders *****/

typedef struct struct_vshader VertexShader_t;
typedef struct struct_pshader FragmentShader_t;
typedef struct struct_program ShaderProgram_t;

/*! \brief load a vertex shader from a file.
 *  \param file the name of the shader source file
 *  \returns a pointer to the vertex shader.
 *
 * This function loads and compiles a vertex shader from a file.  If there is
 * an error, it will print information to stderr and return 0.
 */
extern VertexShader_t *LoadVertexShader (const char *file);

/*! \brief free the resources used by a vertex shader.
 *  \param vsh the shader to free.
 */
extern void FreeVertexShader (VertexShader_t *vsh);

/*! \brief load a fragment shader from a file.
 *  \param file the name of the shader source file
 *  \returns a pointer to the fragment shader.
 *
 * This function loads and compiles a fragment shader from a file.  If there is
 * an error, it will print information to stderr and return 0.
 */
extern FragmentShader_t *LoadFragmentShader (const char *file);

/*! \brief free the resources used by a fragment shader.
 *  \param fsh the shader to free.
 */
extern void FreeFragmentShader (FragmentShader_t *fsh);

/*! \brief create a shader program.
 *  \param vsh the vertex shader to use
 *  \param fsh the fragment shader to use
 *  \returns the linked shader program.
 *
 * Create a shader program from the given vertex and fragment shaders.  If either
 * \a vsh or \a fsh is 0, then the fixed pipline is used for the corresponding stage
 * instead.
 */
extern ShaderProgram_t *CreateShaderProgram (VertexShader_t *vsh, FragmentShader_t *fsh);

/*! \brief free the resources used by a shader program.
 *  \param prog the program to free.
 */
extern void FreeShaderProgram (ShaderProgram_t *prog);

/*! \brief use the given shader program for rendering.
 *  \param prog the shader program to use
 */
extern void UseProgram (ShaderProgram_t *prog);

/*! \brief return the location of the given attribute in the given program.
 *  \param prog the program
 *  \param name the name of the attribute
 *  \return the location of the attribute
 */
extern int AttributeLocation (ShaderProgram_t *prog, const char *name);

/*! \brief return the location of the given uniform variable in the given program.
 *  \param prog the program
 *  \param name the name of the uniform variable
 *  \return the location of the uniform variable
 */
extern int UniformLocation (ShaderProgram_t *prog, const char *name);

#endif /* !_SHADER_H_ */
