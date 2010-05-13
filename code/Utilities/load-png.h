/*! \file load-png.h
 *
 * \brief This file defines a simple texture loading API on top of libpng.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _LOAD_PNG_H_
#define _LOAD_PNG_H_

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#endif
#include <stdint.h>
#include <stdbool.h>

//! \brief image formats
typedef enum {
    GRAY_IMAGE = GL_LUMINANCE,			/*!< grayscale */
    GRAY_ALPHA_IMAGE = GL_LUMINANCE_ALPHA,	/*!< grayscale with alpha channel */
    RGB_IMAGE = GL_RGB,				/*!< RGB image */
    RGBA_IMAGE = GL_RGBA			/*!< RGBA image */
} ImageFormat_t;

//! \brief a 2D image
typedef struct {
    uint32_t		wid;	//!< the image width
    uint32_t		ht;	//!< the image height
    ImageFormat_t	fmt;	//!< the format of the image
    GLenum		type;	//!< the type of the channel data; will be either GL_UNSIGNED_BYTE
				//!  or GL_UNSIGNED_SHORT.
    uint8_t		*data;	//!< the image data
} Image2D_t;

/*! \brief Load an image with the requested format from the named file.
 *  \param file the path to the PNG file to be loaded
 *  \param fmt the requested format of the file.
 *  \return the loaded image or 0 if there was an error loading the file.
 */
extern Image2D_t *LoadImage (const char *file, bool flip, ImageFormat_t fmt);

/*! \brief Free the storage allocated for an image.
 *  \param img the image to be freed.
 */
extern void FreeImage (Image2D_t *img);

/*! \brief specify a two-dimensional texture image for the current texture unit.
 *  \param img the image to load into the texture unit.
 *  \return returns 0 (GL_NO_ERROR) if there is no error; otherwise returns the
 *          OpenGL error code.
 *
 * To use this function, you should us glBindTexture to make current the OpenGL texture
 * that you wish to load.
 */
extern int TexImage (Image2D_t *img);

#endif /* !_LOAD_PNG_H_ */
