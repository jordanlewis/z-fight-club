#include "vec3f.h"
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <deque>

//#define EPSILON 2e-6 this gets defined in vector.h now

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

Vec3f::Vec3f(const Vec3f_t &v)
{
     //cout << " -:  " << x << " " << y << " " << z << endl;

    if(abs(v[0]) < EPSILON) this->x = 0;
    else this->x = v[0];

    if(abs(v[1]) < EPSILON) this->y = 0;
    else this->y = v[1];

    if(abs(v[2]) < EPSILON) this->z = 0;
    else this->z = v[2];
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
     // So apparently division by 0 is well defined:
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
    return (abs(x - o.x) < EPSILON &&
            abs(y - o.y) < EPSILON &&
            abs(z - o.z) < EPSILON);
}

// --------- Normalization ---------
Vec3f Vec3f::unit() const
{
    float len = length();
    if (len == 0)
        return Vec3f(0,0,0);
    else
        return Vec3f(x/len,y/len,z/len);
}

void Vec3f::normalize()
{
    float len = length();

    if (len != 0)
    {
        x /= len;
        y /= len;
        z /= len;
    }
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

// Project this onto v
Vec3f Vec3f::project_onto(const Vec3f& v) const
{
    Vec3f vn = v.unit();
    return vn * vn.dot(*this);
}

Vec3f Vec3f::perp() const
{
    Vec3f u = Vec3f(1.0, 0.0, 0.0);

    /* check to see if this vector lies in the X-axis*/
    if (((*this) * u).length() < EPSILON)
    {
        return Vec3f(0.0, 1.0, 0.0);
    }

    u -= u.project_onto(*this);
    u.normalize();
    return u;
}

Vec3f Vec3f::perp(const Vec3f &v) const
{
    Vec3f p = ((*this) * v);
    p.normalize();
    return p;
}

// --------- Interpolation ---------

Vec3f lerp(const Vec3f &v0, const Vec3f &v1, const float &t) {
    return t * v1 + (1 - t) * v0;
}

Vec3f slerp(const Vec3f &v0, const Vec3f &v1, const float &t) {
    Vec3f temp1 = v0.unit(), temp2 = v1.unit();
    float theta = acos(temp1.dot(temp2));
    return (sin((1.0f - t) * theta) / sin(theta)) * v0 + (sin(t * theta) / sin(theta)) * v1;
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

// --------- Vertex Array Functions --------
float *makeArray(const std::vector<Vec3f> array)
{
    unsigned int i, j;
    float *verts = new float[3 * array.size()];

    for (i = 0; i < array.size(); i++)
        for (j = 0; j < 3; j++)
            verts[i*3 + j] = array[i][j];

    return verts;
}

float *makeArray(std::deque<Vec3f> array)
{
    int j;

    float *verts = new float[3 * array.size()];

    for(deque<Vec3f>::iterator i = array.begin(); i != array.end(); i++)
        for (j = 0; j < 3; j++)
            verts[distance(array.begin(), i) * 3 + j] = (*i)[j];

    return verts;
}

float *makeArray(Vec3f * array, int nvecs)
{
    float *verts = new float[3 * nvecs];

    int i, j;
    for (i = 0; i < nvecs; i++)
    {
        for (j = 0; j < 3; j++)
            verts[i * 3 + j] = array[i][j];
    }

    return verts;
}

void Vec3f::toArray(float *array)
{
    array[0] = x;
    array[1] = y;
    array[2] = z;
}

Vec3f randomVec3f(Vec3f scaleFactor)
{
    float x = (float) rand() / (float) RAND_MAX;
    x *= 2;
    x -= 1;
    x *= scaleFactor[0];

    float y = (float) rand() / (float) RAND_MAX;
    y *= 2;
    y -= 1;
    y *= scaleFactor[1];

    float z = (float) rand() / (float) RAND_MAX;
    z *= 2;
    z -= 1;
    z *= scaleFactor[2];

    return Vec3f(x,y,z);
}

#undef EPSILON
