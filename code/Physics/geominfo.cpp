#include <ode/ode.h>
#include "geominfo.h"

#define PI 3.1415

GeomInfo::GeomInfo(float bounce, float mu1, float mu2)
    : bounce(bounce), mu1(mu1), mu2(mu2)
{}

PlaneInfo::PlaneInfo(float a, float b, float c, float d, 
		     float bounce, float mu1, float mu2, dSpaceID space)
    : GeomInfo(bounce, mu1, mu2), space(space), a(a), b(b), c(c), d(d)
{}
BoxInfo::BoxInfo(float lx, float ly, float lz, 
		 float bounce, float mu1, float mu2, dSpaceID space)
    : GeomInfo(bounce, mu1, mu2), space(space), lx(lx), ly(ly), lz(lz)
{}
SphereInfo::SphereInfo(float radius, 
		       float bounce, float mu1, float mu2, dSpaceID space)
    : GeomInfo(bounce, mu1, mu2), space(space), radius(radius)
{}

TriMeshInfo::TriMeshInfo(dTriMeshDataID meshID, const void * verts,
                         const void * tris, float bounce, float mu1,
                         float mu2, dSpaceID space)
    : GeomInfo(bounce, mu1, mu2), space(space), meshID(meshID),
      verts(verts), tris(tris)
{}

dGeomID SphereInfo::createGeom() { return dCreateSphere (space, radius); }
dGeomID BoxInfo::createGeom()    { return dCreateBox(space, lx, ly, lz); }
dGeomID PlaneInfo::createGeom()  { return dCreatePlane(space, a, b, c, d); }
dGeomID TriMeshInfo::createGeom(){ return dCreateTriMesh(space, meshID,
                                                         NULL, NULL, NULL); }

void SphereInfo::createMass(dMass * mass, float massVal)
{
    //dMassSetSphere(mass, massVal/((4/3)*PI*radius*radius*radius), radius);
    dMassSetSphereTotal(mass, massVal, radius);
}

void BoxInfo::createMass(dMass * mass, float massVal)
{
    //dMassSetBox(mass, massVal/(lx*ly*lz), lx, ly, lz);
    dMassSetBoxTotal(mass, massVal, lx, ly, lz);
}
