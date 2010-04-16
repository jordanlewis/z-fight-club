#include "vec3f.h"
#include <cmath>

#define EPSILON 2e-6

// --------- Constructors --------- 

Vec3f::Vec3f()
{
    this->x = this->y = this->z = 0.0;
}

Vec3f::~Vec3f() {}

#include <iostream>
using namespace std;
Vec3f::Vec3f(float x, float y, float z)
{
    //cout << " -:  " << x << " " << y << " " << z << endl;

    if(abs(x) < EPSILON) this->x = 0;
    else this->x = x;

    if(abs(y) < EPSILON) this->y = 0;
    else this->y = y;
    
    if(abs(z) < EPSILON) this->z = 0;
    else this->z = z;

    //cout << " -:: " << this->x << " " << this->y << " " << this->z << endl;
}

// flattens small vectors to avoid floating point errors
void Vec3f::flatten()
{
    if(abs(x) < EPSILON) this->x = 0;
    else this->x = x;

    if(abs(y) < EPSILON) this->y = 0;
    else this->y = y;
    
    if(abs(z) < EPSILON) this->z = 0;
    else this->z = z;
}

// --------- Addition --------- 

Vec3f Vec3f::operator+(const Vec3f &o) const
{
    return Vec3f(x+o.x,y+o.y,z+o.z);
}

void Vec3f::operator+=(const Vec3f &o)
{
    x += o.x;
    y += o.y;
    z += o.z;

    flatten();
}

// --------- Subtraction --------- 

Vec3f Vec3f::operator-(const Vec3f &o) const
{
    return Vec3f(x-o.x,y-o.y,z-o.z);
}

void Vec3f::operator-=(const Vec3f &o)
{
    x -= o.x;
    y -= o.y;
    z -= o.z;

    flatten();
}

// --------- Cross Product --------- 

Vec3f Vec3f::operator*(const Vec3f &o) const
{
    return Vec3f( (y*o.z) - (z*o.y),
                  (z*o.x) - (x*o.z),
		  (x*o.y) - (y*o.x)
		);
}

void Vec3f::operator*=(const Vec3f &o)
{
    // I don't think you can do cross product in-place,
    // so I fgure I might as well factor out the code.
    Vec3f cross = operator*(o);
    x = cross.x;
    y = cross.y;
    z = cross.z;

}

// --------- Scalar Multiplication --------- 

Vec3f Vec3f::operator*(const float &o) const
{
    return Vec3f(x*o,y*o,z*o);
}

void Vec3f::operator*=(const float &o)
{
    if(o == 0) {
        x = y = z = 0;
    } else {
        x *= o;
	y *= o;
	z *= o;

	flatten();
    }
}

// --------- Scalar Division --------- 

Vec3f Vec3f::operator/(const float &o) const
{
     // So apparently, in C++, division by 0 is well defined:
     // 1.0/0.0 = infinity.
     //
     // So, because it's well-defined for the system, I'm not
     // going to throw an exception when I encounter it.

     return Vec3f(x/o,y/o,z/o);
}

void Vec3f::operator/=(const float &o)
{
    x /= o;
    y /= o;
    z /= o;
    
    flatten();
}

// --------- Negation --------- 

Vec3f Vec3f::operator-() const
{
    return Vec3f(-x,-y,-z);
}

// --------- Element Access --------- 
    
float &Vec3f::operator[](int index)
{
    switch(index%3) {
      case 0:
        return x;
      case 1:
        return y;
      default: // index%3 == 2
        return z;
    }
}

float Vec3f::operator[](int index) const
{
    switch(index%3) {
      case 0:
        return x;
      case 1:
        return y;
      default: // index%3 == 2
        return z;
    }
}

// --------- Equality --------- 

bool Vec3f::operator==(const Vec3f &o) const
{
    return (abs(x == o.x) < EPSILON &&
            abs(y == o.y) < EPSILON &&
	    abs(z == o.z) < EPSILON);
}

// --------- Normalization --------- 
Vec3f Vec3f::norm() const
{
    float len = length();
    return Vec3f(x/len,y/len,z/len);
}

void Vec3f::normalizeMe()
{
    float len = length();
    x /= len;
    y /= len;
    z /= len;
}

// --------- Additional Functions --------- 

float Vec3f::length() const
{
    return sqrt((x*x)+(y*y)+(z*z));
}

float Vec3f::dot(const Vec3f &o) const
{
    return ((x*o.x) + (y*o.y) + (z*o.z));
}

// --------- Stream Output --------- 

std::ostream &operator<<(std::ostream &os, const Vec3f &o)
{
    os << "(" << o.x << "," << o.y << "," << o.z << ")";
    return os;
}

// --------- Scalar Multiplication --------- 

Vec3f operator*(const float &s, const Vec3f &v)
{
    return Vec3f(s*v.x,s*v.y,s*v.z);
}

#undef EPSILON
