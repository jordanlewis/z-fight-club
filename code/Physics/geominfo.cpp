#include <ode/ode.h>
#include "geominfo.h"

#define PI 3.1415

PlaneInfo::PlaneInfo(float a, float b, float c, float d, dSpaceID space)
                    : space(space), a(a), b(b), c(c), d(d)
{}
BoxInfo::BoxInfo(float lx, float ly, float lz, dSpaceID space)
                : space(space), lx(lx), ly(ly), lz(lz)
{}
SphereInfo::SphereInfo(float radius, dSpaceID space)
                      : space(space), radius(radius)
{}

dGeomID SphereInfo::createGeom() { return dCreateSphere (space, radius); }
dGeomID BoxInfo::createGeom()    { return dCreateBox(space, lx, ly, lz); }
dGeomID PlaneInfo::createGeom()  { return dCreatePlane(space, a, b, c, d); }

void SphereInfo::createMass(dMass * mass, float massVal)
{
    dMassSetSphere(mass, massVal/(2*PI*radius), radius);
    
    //dMassSetSphereTotal(mass, massVal, radius);
}

void BoxInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBox(mass, massVal/(lx*ly*lz), lx, ly, lz);
    //dMassSetBoxTotal(mass, massVal, lx, ly, lz);
}
