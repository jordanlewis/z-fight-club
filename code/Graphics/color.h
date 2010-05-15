/*! \file color.h
 *
 * \brief Various types and operations for colors.
 *
 * \author John Reppy
 */

/* COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _COLOR_H_
#define _COLOR_H_

typedef float           Color3_t[4];    //!< floating-point RGB colors
typedef float           Color4_t[4];    //!< floating-point RGBA colors
typedef unsigned char   RGB_t[3];       //!< Byte RGB colors
typedef unsigned char   RGBA_t[4];      //!< Byte RGBA colors

//! \brief copy a color
//! \param src the source color
//! \param dst the destination color
static inline void CopyColor3 (Color3_t src, Color3_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

//! \brief copy a color
//! \param src the source color
//! \param dst the destination color
static inline void CopyColor4 (Color4_t src, Color4_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

//! \brief copy an RGBA color
//! \param src the source color
//! \param dst the destination color
static inline void CopyRGBA (RGBA_t src, RGBA_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

//! \brief copy an RGB color
//! \param src the source color
//! \param dst the destination color
static inline void CopyRGB (RGB_t src, RGB_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

#endif /* !_COLOR_H_ */
