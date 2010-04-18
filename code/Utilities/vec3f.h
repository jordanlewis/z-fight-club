#ifndef VEC3F_H
#define VEC3F_H

#include <iostream>

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
    float &operator[](int index);

    //! \brief returns the ith element of this vector
    float operator[](int index) const;

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
};

// --------- Scalar Multiplication --------- 
//! \brief Multiplies a vector by a scalar and returns the result.
//         This allows for the more traditional notation s * v
Vec3f operator*(const float &s, const Vec3f&);

// --------- Stream Output --------- 
//! \brief Writes this vector to the given output stream.
//         This will allow us to debug with cout << Vec3f
std::ostream &operator<<(std::ostream&, const Vec3f&);

#endif
