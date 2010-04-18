#include <ode/ode.h>
#include "geominfo.h"

dGeomID SphereInfo::createGeom() { return dCreateSphere (space, radius); }
dGeomID BoxInfo::createGeom()    { return dCreateBox(space, lx, ly, lz); }
dGeomID PlaneInfo::createGeom()  { return dCreatePlane(space, a, b, c, d); }

void SphereInfo::createMass(dMass * mass, float massVal)
{
    dMassSetSphereTotal(mass, massVal, radius);
}

void BoxInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, lx, ly, lz);
}
