/*! \file load-png.c
 *
 * \brief This file implements a simple texture loading API on top of libpng.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#include <png.h>
#include "defs.h"
#include "load-png.h"
#include <assert.h>

/* LoadImage:
 */
Image2D_t *LoadImage (const char *file, bool flip, ImageFormat_t fmt)
{
  // open image file
    FILE *inS = fopen(file, "rb");
    if (inS == NULL) {
	fprintf (stderr, "unable to open \"%s\"\n", file);
	return 0;
    }

  /* check PNG signature */
    unsigned char sig[8];
    if ((fread(sig, 1, 8, inS) != 8)
    || (png_sig_cmp(sig, 0, 8))) {
	fprintf (stderr, "error reading \"%s\": bad signature\n", file);
	fclose(inS);
	return 0;
    }

  /* setup read structures */
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (pngPtr == 0) {
	fprintf (stderr, "error reading \"%s\"\n", file);
	fclose(inS);
	return 0;
    }
    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (infoPtr == 0) {
	fprintf (stderr, "error reading \"%s\"\n", file);
	png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
	fclose(inS);
	return 0;
    }
    png_infop endPtr = png_create_info_struct(pngPtr);
    if (!endPtr) {
	fprintf (stderr, "error reading \"%s\"\n", file);
	png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	fclose (inS);
	return 0;
    }

  /* error handler */
    if (setjmp (png_jmpbuf(pngPtr))) {
	fprintf (stderr, "error reading \"%s\"\n", file);
	png_destroy_read_struct (&pngPtr, &infoPtr, &endPtr);
	fclose (inS);
	return 0;
    }

  /* set the input stream */
    png_init_io (pngPtr, inS);

  /* let the PNG library know that we already checked the signature */
    png_set_sig_bytes (pngPtr, 8);

  /* get file info */
    png_uint_32 width, height;
    int bitDepth, colorType;
    png_read_info (pngPtr, infoPtr);
    png_get_IHDR (pngPtr, infoPtr, &width, &height,
	&bitDepth, &colorType, 0 /* interlace type */,
	0 /* compression type */, 0 /* filter method */);

  // check file format against expected format
    switch (colorType) {
      case PNG_COLOR_TYPE_GRAY:
	if (bitDepth < 8) {
	    png_set_expand_gray_1_2_4_to_8(pngPtr);
	}
	if (fmt != GRAY_IMAGE) {
	    fprintf(stderr, "unexpected GRAY image format\n");
	    png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	    fclose (inS);
	    return 0;
	}
	break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
	if (fmt != GRAY_ALPHA_IMAGE) {
	    fprintf(stderr, "unexpected GRAY_ALPHA image format\n");
	    png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	    fclose (inS);
	    return 0;
	}
	break;
      case PNG_COLOR_TYPE_PALETTE:
	png_set_palette_to_rgb (pngPtr);
	if (fmt != RGB_IMAGE) {
	    fprintf(stderr, "unexpected PALETTE image format\n");
	    png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	    fclose (inS);
	    return 0;
	}
	break;
      case PNG_COLOR_TYPE_RGB:
	if (fmt != RGB_IMAGE) {
	    fprintf(stderr, "unexpected RGB image format\n");
	    png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	    fclose (inS);
	    return 0;
	}
	break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
	if (fmt != RGBA_IMAGE) {
	    fprintf(stderr, "unexpected RGBA image format\n");
	    png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	    fclose (inS);
	    return 0;
	}
	break;
      default:
	fprintf(stderr, "unknown color type %d\n", colorType);
	png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	fclose (inS);
	return 0;
    }

  // figure out the OpenGL image type
    GLenum type;
    if (bitDepth <= 8) type = GL_UNSIGNED_BYTE;
    else if (bitDepth == 16) type = GL_UNSIGNED_BYTE;
    else {
	fprintf(stderr, "unsupported bit depth\n");
	png_destroy_read_struct (&pngPtr, &infoPtr, (png_infopp)0);
	fclose (inS);
	return 0;
    }

  /* allocate image data */
    unsigned char *imgData = 0;
    png_bytepp rowPtrs = 0;
    int bytesPerRow = png_get_rowbytes(pngPtr, infoPtr);
    if ((imgData = NEWVEC(unsigned char, height * bytesPerRow)) == 0) {
	png_destroy_read_struct (&pngPtr, &infoPtr, &endPtr);
	free (rowPtrs);
	fclose (inS);
	return 0;
    }

    rowPtrs = (png_bytepp) malloc (height * sizeof(png_bytep));
    if (rowPtrs == 0) {
	png_destroy_read_struct (&pngPtr, &infoPtr, &endPtr);
	free (imgData);
	fclose (inS);
	return 0;
    }

    if (flip) {
      /* setup row pointers */
	for (int i = 0;  i < height;  i++)
	    rowPtrs[i] = imgData + i*bytesPerRow;
    }
    else {
      /* setup row pointers so that the texture has OpenGL orientation */
	for (int i = 1;  i <= height;  i++)
	    rowPtrs[height - i] = imgData + (i-1)*bytesPerRow;
    }

  /* read the image */
    png_read_image(pngPtr, rowPtrs);

  /* Clean up. */
    png_destroy_read_struct (&pngPtr, &infoPtr, &endPtr);
    free (rowPtrs);
    fclose (inS);

  /* allocate and initialize the image data structure */
    Image2D_t *img = NEW(Image2D_t);
    img->wid	= width;
    img->ht	= height;
    img->fmt	= fmt;
    img->type	= type;
    img->data	= imgData;

    return img;

} /* end of LoadImage */

/* FreeImage:
 */
void FreeImage (Image2D_t *img)
{
    if (img == 0) return;

    free (img->data);
    free (img);

} /* end of FreeImage */

/* TexImage:
 */
int TexImage (Image2D_t *img)
{
    assert (img != 0);

    glTexImage2D (GL_TEXTURE_2D, 0, img->fmt, img->wid, img->ht, 0, img->fmt, img->type, img->data);

    return (glGetError());

} /* end of TexImage */

void DrawImage(Image2D_t *img, float x, float y)
{
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    assert(img);
    /* we got an image we're good to go */
    glRasterPos2f(x,y);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawPixels(img->wid, img->ht, img->fmt, img->type, img->data);  

    glDisable(GL_BLEND);
}
