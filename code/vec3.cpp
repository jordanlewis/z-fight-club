#include "vec3.h"

Vec3f::Vec3f() {
    this->x = this->y = this->z = 0.0;
}

Vec3f::Vec3f(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3f Vec3f::operator+(const Vec3f &o) const {
    return Vec3f(x+o.x,y+o.y,z+o.z);
}

void Vec3f::operator+=(const Vec3f &o) {
    x += o.x;
    y += o.y;
    z += o.z;
}

std::ostream &operator<<(std::ostream &os, const Vec3f &o) {
    os << "(" << o.x << "," << o.y << "," << o.z << ")";
    return os;
}
