#include <ode/ode.h>
#include "geominfo.h"

PlaneInfo::PlaneInfo(float a, float b, float c, float d)
    : GeomInfo(), a(a), b(b), c(c), d(d)
{}
BoxInfo::BoxInfo(float lx, float ly, float lz)
    : GeomInfo(), lx(lx), ly(ly), lz(lz)
{}
SphereInfo::SphereInfo(float radius)
    : GeomInfo(), radius(radius)
{}

TriMeshInfo::TriMeshInfo(dTriMeshDataID meshID, const void * verts,
                         const void * tris)
    : GeomInfo(), meshID(meshID), verts(verts), tris(tris)
{}

dGeomID SphereInfo::createGeom(dSpaceID space)
{ return dCreateSphere (space, radius); }
dGeomID BoxInfo::createGeom(dSpaceID space)
{ return dCreateBox(space, lx, ly, lz); }
dGeomID PlaneInfo::createGeom(dSpaceID space)
{ return dCreatePlane(space, a, b, c, d); }
dGeomID TriMeshInfo::createGeom(dSpaceID space)
{ return dCreateTriMesh(space, meshID, NULL, NULL, NULL); }

void SphereInfo::createMass(dMass * mass, float massVal)
{
    dMassSetSphereTotal(mass, massVal, radius);
}

void BoxInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, lx, ly, lz);
}
