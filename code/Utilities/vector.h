/*! \file vector.h
 *
 * \brief Various types and operations for 3D vectors.
 *
 * \author John Reppy
 */

/* CMSC23700 Sample code
 *
 * COPYRIGHT (c) 2010 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <stdlib.h>

/*! \brief a small value for testing if something is close to 0 */
#define EPSILON 1e-6

typedef float        Vec2f_t[2];    //!< 2D vector (used for texture coords)
typedef float        Vec3f_t[3];    //!< 3D vector
typedef float        Vec4f_t[4];    //!< 4D vector
typedef float        Color_t[4];    //!< RGBA color representation

/*! \brief a ray */
typedef struct {
    Vec3f_t    orig;            //!< the origin of the ray
    Vec3f_t    dir;            //!< the direction of the ray, which should be
                    //!< a unit vector
    float len; //The length of the ray.
} Rayf_t;

/*! \brief a plane */
typedef struct {
    Vec3f_t    n;            //!< the plane's unit normal vector
    float    d;            //!< the signed distance to the origin
} Planef_t;


/***** Scalar functions *****/

//! \brief convert degrees to radians
static inline float toRads(const float x) { return ((x*M_PI)/180); }

//! \brief convert radians to degrees
static inline float toDegs(const float x) { return ((x*180)/M_PI); }

//! \brief Square a number
static inline float Sqrf (const float x) { return (x*x); }

//! \brief clamp a float value to [0.0,1.0]
static inline float Clampf (const float x)
{
    if (x < 0.0) return 0.0;
    else if (x > 1.0) return 1.0;
    else return x;
}

/***** 2D functions *****/

//! \brief set a vector to be all 0s
//! \param v the vector
static inline void ZeroV2f (Vec2f_t v)
{
    v[0] = v[1] = 0.0f;
}

//! \brief dot product
//! \param v1 an argument vector
//! \param v2 an argument vector
//! \return the dot produce of \a v1 and \a v2
static inline float DotV2f (const Vec2f_t v1, const Vec2f_t v2)
{
    return ((v1[0]*v2[0]) + (v1[1]*v2[1]));
}

//! \brief return the length squared of a vector
//! \param v the argument vector
//! \return the square of the length of \a v
static inline float LengthSqV2f (const Vec2f_t v)
{
    return DotV2f (v, v);
}

//! vector length
//! \param v the argument vector
//! \return the length of \a v
static inline float LengthV2f (const Vec2f_t v)
{
    return sqrt (LengthSqV2f(v));
}

/*! \brief scalar-vector multiplication
 *  \param s the scalar multiplier
 *  \param v the vector to be scaled
 *  \param dst the result vector
 *
 *  sets the vector \a dst to \a s * \a v.
 */
static inline void ScaleV2f (const float s, const Vec2f_t v, Vec2f_t dst)
{
    dst[0] = s*v[0];
    dst[1] = s*v[1];
}

/*! \brief vector normalization
 *  \param v the vector to normalize
 *  \return the length of \a v
 *
 * This function normalized \a v to be a unit vector and returns the
 * original length of \a v.  If the length of \a v is close to zero,
 * the it is not normalized and 0 is returned.
 */
static inline float NormalizeV2f (Vec2f_t v)
{
    float s = LengthSqV2f(v);
    if (s < EPSILON) return 0.0;
    s = sqrt(s);
    ScaleV2f (1.0 / s, v, v);
    return s;
}

/*! \brief Clamp a vector to the unit square
 *  \param v the vector to be clamped.
 *
 * Clamps each coordinate of \a v to the unit interval.
 */
static inline void ClampV2f (Vec2f_t v)
{
    v[0] = Clampf(v[0]);
    v[1] = Clampf(v[1]);
}

/*! \brief vector negation
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to the negation of \a v
 */
static inline void NegV2f (const Vec2f_t v, Vec2f_t dst)
{
    dst[0] = -v[0];
    dst[1] = -v[1];
}

/*! \brief vector copy
 *  \param src the source vector
 *  \param dst the destination vector
 *
 * sets the vector \a dst to \a src
 */
static inline void CopyV2f (const Vec2f_t src, Vec2f_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/*! \brief vector addition
 *  \param u an argument vector
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to \a u + \a v
 */
static inline void AddV2f (const Vec2f_t u, const Vec2f_t v, Vec2f_t dst)
{
    dst[0] = u[0] + v[0];
    dst[1] = u[1] + v[1];
}

/*! \brief vector subtraction
 *  \param u an argument vector
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to \a u - \a v
 */
static inline void SubV2f (const Vec2f_t u, const Vec2f_t v, Vec2f_t dst)
{
    dst[0] = u[0] - v[0];
    dst[1] = u[1] - v[1];
}

/*! \brief scaled vector addition
 *  \param u an argument vector
 *  \param s the scaling factor
 *  \param v an argument vector
 *  \param dst the result vector
 *
 *  sets the vector \a dst to \a u + \a s * \a v
 */
static inline void ScaledAddV2f (const Vec2f_t u, const float s, const Vec2f_t v, Vec2f_t dst)
{
    dst[0] = u[0] + s*v[0];
    dst[1] = u[1] + s*v[1];
}


/***** 3D functions *****/

//! \brief set a vector to be all 0s
//! \param v the vector
static inline void ZeroV3f (Vec3f_t v)
{
    v[0] = v[1] = v[2] = 0.0f;
}

//! \brief 3D vector dot product
//! \param v1 an argument vector
//! \param v2 an argument vector
//! \return the dot produce of \a v1 and \a v2
static inline float DotV3f (const Vec3f_t v1, const Vec3f_t v2)
{
    return ((v1[0]*v2[0]) + (v1[1]*v2[1]) + (v1[2]*v2[2]));
}

//! \brief return the length squared of a 3D vector
//! \param v the argument vector
//! \return the square of the length of \a v
static inline float LengthSqV3f (const Vec3f_t v)
{
    return DotV3f (v, v);
}

//! vector length
//! \param v the argument vector
//! \return the length of \a v
static inline float LengthV3f (Vec3f_t v)
{
    return sqrt (LengthSqV3f(v));
}

/*! \brief scalar-vector multiplication
 *  \param s the scalar multiplier
 *  \param v the vector to be scaled
 *  \param dst the result vector
 *
 *  sets the vector \a dst to \a s * \a v.
 */
static inline void ScaleV3f (float s, Vec3f_t v, Vec3f_t dst)
{
    dst[0] = s*v[0];
    dst[1] = s*v[1];
    dst[2] = s*v[2];
}

/*! \brief vector normalization
 *  \param v the vector to normalize
 *  \return the length of \a v
 *
 * This function normalized \a v to be a unit vector and returns the
 * original length of \a v.  If the length of \a v is close to zero,
 * the it is not normalized and 0 is returned.
 */
static inline float NormalizeV3f (Vec3f_t v)
{
    float s = LengthSqV3f(v);
    if (s < EPSILON) return 0.0;
    s = sqrt(s);
    ScaleV3f (1.0 / s, v, v);
    return s;
}

/*! \brief Clamp a vector to the unit cube
 *  \param v the vector to be clamped.
 *
 * Clamps each coordinate of \a v to the unit interval.
 */
static inline void ClampV3f (Vec3f_t v)
{
    v[0] = Clampf(v[0]);
    v[1] = Clampf(v[1]);
    v[2] = Clampf(v[2]);
}

/*! \brief vector negation
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to the negation of \a v
 */
static inline void NegV3f (Vec3f_t v, Vec3f_t dst)
{
    dst[0] = -v[0];
    dst[1] = -v[1];
    dst[2] = -v[2];
}

/*! \brief vector copy
 *  \param src the source vector
 *  \param dst the destination vector
 *
 * sets the vector \a dst to \a src
 */
static inline void CopyV3f (Vec3f_t src, Vec3f_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

/*! \brief vector addition
 *  \param u an argument vector
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to \a u + \a v
 */
static inline void AddV3f (Vec3f_t u, Vec3f_t v, Vec3f_t dst)
{
    dst[0] = u[0] + v[0];
    dst[1] = u[1] + v[1];
    dst[2] = u[2] + v[2];
}

/*! \brief scaled vector addition
 *  \param u an argument vector
 *  \param s the scaling factor
 *  \param v an argument vector
 *  \param dst the result vector
 *
 *  sets the vector \a dst to \a u + \a s * \a v
 */
static inline void ScaledAddV3f (Vec3f_t u, float s, Vec3f_t v, Vec3f_t dst)
{
    dst[0] = u[0] + s*v[0];
    dst[1] = u[1] + s*v[1];
    dst[2] = u[2] + s*v[2];
}

/*! \brief vector subtraction
 *  \param u an argument vector
 *  \param v an argument vector
 *  \param dst the result vector
 *
 * sets the vector \a dst to \a u - \a v
 */
static inline void SubV3f (const Vec3f_t u, const Vec3f_t v, Vec3f_t dst)
{
    dst[0] = u[0] - v[0];
    dst[1] = u[1] - v[1];
    dst[2] = u[2] - v[2];
}

/*! \brief linear interpolation between two vectors.
 *  \param u an argument vector
 *  \param s the scaling factor
 *  \param v an argument vector
 *  \param dst the result vector
 *
 *  sets the vector \a dst to (1-t)*\a u + \a t * \a v
 */
static inline void LerpV3f (Vec3f_t u, float t, Vec3f_t v, Vec3f_t dst)
{
    dst[0] = u[0] + t*(v[0] - u[0]);
    dst[1] = u[1] + t*(v[1] - u[1]);
    dst[2] = u[2] + t*(v[2] - u[2]);
}

/*! \brief vector cross product
 *  \param v1 an argument vector
 *  \param v2 an argument vector
 *  \param dst the result vector
 *
 *  sets the vector \a dst to the cross product of \a v1 and \a v2.
 */
static inline void CrossV3f (Vec3f_t v1, Vec3f_t v2, Vec3f_t dst)
{
    float x = v1[1]*v2[2] - v1[2]*v2[1];
    float y = v1[2]*v2[0] - v1[0]*v2[2];
    float z = v1[0]*v2[1] - v1[1]*v2[0];
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
}

/***** 4D functions *****/

//! \brief set a vector to be all 0s
//! \param v the vector
static inline void ZeroV4f (Vec2f_t v)
{
    v[0] = v[1] = v[2] = v[3] = 0.0f;
}


/*! \brief vector copy
 *  \param src the source vector
 *  \param dst the destination vector
 *
 * sets the vector \a dst to \a src
 */
static inline void CopyV4f (Vec4f_t src, Vec4f_t dst)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

/***** Ray functions *****/

//! return a point on a ray
//! \param r the ray
//! \param t the distance along the ray
//! \param dst the resulting point
static inline void RayToPointf (Rayf_t *r, float t, Vec3f_t dst)
{
    ScaledAddV3f (r->orig, t, r->dir, dst);
}


/***** Planef_t operations *****/

//! \brief initialize a plane from a normal vector and distance.
//! \param norm the plane's unit normal vector
//! \param d the signed distance to the origin
//! \param plane the plane to be initialized.
static inline void InitPlanef (Vec3f_t norm, float d, Planef_t *plane)
{
    CopyV3f (norm, plane->n);
    plane->d = d;

}

//! \brief return the signed distance from a point to a plane
//! \param plane the plane
//! \param pt the point
//! \return the signed distance
static inline float DistanceToPlanef (const Planef_t *plane, const Vec3f_t pt)
{
    return DotV3f (plane->n, pt) + plane->d;
}

//! \brief generate a random float between 0.0 and 1.0.
static float ranf ()
{
    return (float)drand48();
}

//! \brief generate a random unit vector.
//  \param v this output parameter will hold the random vector.
static inline void RandVec (Vec3f_t v)
{
    do {
    v[0] = 2.0 * ranf() - 1.0;
    v[1] = 2.0 * ranf() - 1.0;
    v[2] = 2.0 * ranf() - 1.0;
    } while (LengthSqV3f(v) < EPSILON);
}
#endif /* !_VECTOR_H_ */
