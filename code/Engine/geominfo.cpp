#include <ode/ode.h>
#include "geominfo.h"

BoxInfo::BoxInfo(float lx, float ly, float lz)
    : GeomInfo(), lx(lx), ly(ly), lz(lz)
{}
SphereInfo::SphereInfo(float radius)
    : GeomInfo(), radius(radius)
{}
PlaneInfo::PlaneInfo(float a, float b, float c, float d)
    : GeomInfo(), a(a), b(b), c(c), d(d)
{}
RayInfo::RayInfo(float len)
    : GeomInfo(), len(len)
{}
TriMeshInfo::TriMeshInfo(dTriMeshDataID meshID,
                         int nVerts, const Vec3f_t * verts,
                         int nTris,  const int * tris,
                         Vec3f_t *normals)
    : GeomInfo(), meshID(meshID), nVerts(nVerts), verts(verts), nTris(nTris),
      tris(tris), normals(normals)
{}

dGeomID SphereInfo::createGeom(dSpaceID space)
{ return dCreateSphere (space, radius); }
dGeomID BoxInfo::createGeom(dSpaceID space)
{ return dCreateBox(space, lx, ly, lz); }
dGeomID PlaneInfo::createGeom(dSpaceID space)
{ return dCreatePlane(space, a, b, c, d); }
dGeomID RayInfo::createGeom(dSpaceID space)
{ return dCreateRay(space, len); }
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

void ObjMeshInfo::load(char *filename)
{
    obj.load(filename);
}
