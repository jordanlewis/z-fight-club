#include <ode/ode.h>
#include "geominfo.h"
#include <string>
#include "Network/racerpacket.h"
#include "Engine/world.h"
extern "C" {
    #include "Parser/obj-reader.h"
}

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

ObjMeshInfo::ObjMeshInfo(std::string filename)
{
    World &world = World::getInstance();
    model = OBJReadOBJ((world.assetsDir + filename).c_str());
}

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
dGeomID ObjMeshInfo::createGeom(dSpaceID space)
{ return dCreateSphere (space, 1.0); }

void SphereInfo::createMass(dMass * mass, float massVal)
{
    dMassSetSphereTotal(mass, massVal, radius);
}

void BoxInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, lx, ly, lz);
}

void ObjMeshInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, 1.0, 1.0, 1.0);
}

void SphereInfo::hton(RPGeomInfo *payload) {
    if (payload == NULL){
        cerr << "Payload is null!" << endl;
        return;
    }
    payload->radius = htonf(radius);
}

void BoxInfo::hton(RPGeomInfo *payload) {
    if (payload == NULL){
        cerr << "Payload is null!" << endl;
        return;
    }
    payload->lx = htonf(lx);
    payload->ly = htonf(ly);
    payload->lz = htonf(lz);
}

void PlaneInfo::hton(RPGeomInfo *payload) {
    if (payload == NULL){
        cerr << "Payload is null!" << endl;
        return;
    }
    payload->a = htonf(a);
    payload->b = htonf(b);
    payload->c = htonf(c);
    payload->d = htonf(d);
}

void SphereInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->radius = radius;
}

void BoxInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->lx = lx;
    payload->ly = ly;
    payload->lz = lz;
}

void PlaneInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->a = a;
    payload->b = b;
    payload->c = c;
    payload->d = d;
}

ObjMeshInfo::~ObjMeshInfo()
{
    OBJDelete(model);
}
