#ifndef VEC3_H
#define VEC3_H

#include <iostream>

class Vec3f {
  public:
    float x,y,z;

    // --------- Constructors --------- 
    Vec3f();
    Vec3f(float x, float y, float z);
    ~Vec3f();

    // --------- Addition --------- 

    Vec3f operator+(const Vec3f &o) const;

    void operator+=(const Vec3f &o);
    
    // --------- Subtraction --------- 

    Vec3f operator-(const Vec3f &o) const;

    void operator-=(const Vec3f &o);

    // --------- Cross Product --------- 

    Vec3f operator*(const Vec3f &o) const;

    void operator*=(const Vec3f &o);

    // --------- Scalar Multiplication --------- 

    Vec3f operator*(const float &o) const;

    void operator*=(const float &o);

    // --------- Scalar Division --------- 

    Vec3f operator/(const float &o) const;

    Vec3f operator/=(const float &o);

    // --------- Negation --------- 

    Vec3f operator-() const;

    // --------- Element Access --------- 
    
    float &operator[](int index);

    float operator[](int index) const;


};

// --------- Scalar Multiplication --------- 
// This allows for the more traditional notation s * v;
Vec3f operator*(const float &s, const Vec3f &o);

// --------- Stream Output --------- 
std::ostream &operator<<(std::ostream &out,Vec3f &o);

#endif
