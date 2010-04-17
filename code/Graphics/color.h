/*! \file color.h
 *
 * \brief Various types and operations for colors.
 *
 * \author John Reppy
 */

/* COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef COLOR_H_
#define COLOR_H_

Class Color
{
    int dimension; /* !< 3 or four fields */
  public:
    float R, G, B, A;
    Color();
    Color(float, float, float);
    Color(float, float, float, float);
    ~Color();
}
#endif /* !COLOR_H_ */
