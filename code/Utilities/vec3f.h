#ifndef VEC3F_H
#define VEC3F_H

#include <iostream>
#include <vector>
#include <deque>
#include "vector.h"

class Vec3f {
    /*! \brief Checks if the vector is very small, and, if so, sets it to 0
               This corrects for floating point errors and makes sure that
               when someone uses a zero-vector inappropriately, the compiler
               will complain
    */
    void flatten();
  public:
    float x,y,z;

    // --------- Constructors ---------
    Vec3f();
    Vec3f(float, float, float);
    Vec3f(const Vec3f_t&);
    ~Vec3f();

    // --------- Addition ---------

    //! \brief adds two vectors and returns the result
    Vec3f operator+(const Vec3f&) const;

    //! \brief adds two vectors with assignment
    void operator+=(const Vec3f&);

    // --------- Subtraction ---------

    //! \brief subtracts two vectors and returns the result
    Vec3f operator-(const Vec3f&) const;

    //! \brief subtracts two vectors with assignment
    void operator-=(const Vec3f&);

    // --------- Cross Product ---------

    //! \brief takes the cross-product of two vectors and returns the result
    Vec3f operator*(const Vec3f&) const;

    //! \brief takes the cross-product of two vectors and stores the result
    //         in the current vector
    void operator*=(const Vec3f&);

    // --------- Scalar Multiplication ---------

    //! \brief multiplies by a scalar and returns the result
    Vec3f operator*(const float&) const;

    //! \brief multiplies by a scalar with assignment
    void operator*=(const float&);

    // --------- Scalar Division ---------

    //! \brief divides by a scalar and returns the result
    Vec3f operator/(const float&) const;

    //! \brief divides by a scalar with assignment
    void operator/=(const float&);

    // --------- Negation ---------

    //! \brief returns the negation of this vector
    Vec3f operator-() const;

    // --------- Element Access ---------

    //! \brief accesses the ith element of this vector
    float &operator[](int);

    //! \brief returns the ith element of this vector
    float operator[](int) const;

    // --------- Equality ---------

    bool operator==(const Vec3f&) const;

    // --------- Normalization ---------

    //! \brief returns the unit vector that points in the direction of this vector
    Vec3f unit() const;

    //! \brief normalizes this vector
    void normalize();

    // --------- Additional Functions ---------

    //! \brief returns the magnitude of this vector
    float length() const;

    //! \brief takes the dot product with another vector
    float dot(const Vec3f&) const;

    //! \brief project the vector along another vector
    Vec3f project_onto(const Vec3f&) const;

    //! \brief return a vector perpendicular to this one
    Vec3f perp() const;

    //! \brief return a vector perpendicular to this one and another one
    Vec3f perp(const Vec3f&) const;

    //! \brief Write out the Vec3f into a float array
    void toArray(float *array);


};

// --------- Create random vector ---------
//! \brief create a random vector scaled by a scaling factor
//
Vec3f randomVec3f(Vec3f);

// --------- Scalar Multiplication ---------
//! \brief Multiplies a vector by a scalar and returns the result.
//         This allows for the more traditional notation s * v
Vec3f operator*(const float&, const Vec3f&);

// --------- Interpolation ---------
//! \brief interpolate linearly between 2 vectors
Vec3f lerp(const Vec3f&, const Vec3f&, const float&);
//! \brief interpolate spherical between 2 vectors
Vec3f slerp(const Vec3f&, const Vec3f&, const float&);

// --------- Stream Output ---------
//! \brief Writes this vector to the given output stream.
//         This will allow us to debug with cout << Vec3f
std::ostream &operator<<(std::ostream&, const Vec3f&);

// ---------- Vertex Array ---------
//! \brief Create an array of floats usable by opengl
//         out of an array of Vec3fs
float *makeArray(const std::vector<Vec3f>);

//! \brief create an array of floats usable by opengle
//         out of a deque of Vec3fs
float *makeArray(std::deque<Vec3f>);

//! \brief create an array of floats usable by opengl out of an array of Vec3fs
float *makeArray(Vec3f * array, int nvecs);

#endif
