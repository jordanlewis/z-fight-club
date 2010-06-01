#include <ode/ode.h>
#include <stdlib.h>
#include "geominfo.h"
#include <string>
#include "Network/racerpacket.h"
#include "Engine/world.h"
#include "Graphics/gobject.h"
#include "Physics/pobject.h"
extern "C" {
    #include "Parser/obj-reader.h"
    #include "Utilities/load-png.h"
    #include "Utilities/defs.h"
}

BoxInfo::BoxInfo()
    : GeomInfo()
{}
BoxInfo::BoxInfo(float lx, float ly, float lz)
    : GeomInfo(), lx(lx), ly(ly), lz(lz)
{}
SphereInfo::SphereInfo()
    : GeomInfo()
{}
SphereInfo::SphereInfo(float radius)
    : GeomInfo(), radius(radius)
{}
PlaneInfo::PlaneInfo()
    : GeomInfo()
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
    path = filename;
    model = OBJReadOBJ((world.assetsDir + filename + std::string("model.obj")).c_str());
    Image2D_t *color = LoadImage((world.assetsDir + filename + std::string("color.png")).c_str(), false, RGB_IMAGE);
    
    /* Initialize the textures */
    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);
    TexImage(color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    FreeImage(color);
    
   
    /* -------------------------------------------------*/
    /* --- Compile the displaylists for this object --- */
    /* -------------------------------------------------*/
    
    displayList = glGenLists(model->numgroups);
   
    int groupIndex = 0;

    // This will let us configure the rendering options easily
    mode = OBJ_SMOOTH | OBJ_MATERIAL | OBJ_TEXTURE;

    for (OBJgroup *group = model->groups;
         group != NULL;
         group = group->next, groupIndex++) {

         glNewList(displayList + groupIndex,GL_COMPILE);
         glBegin(GL_TRIANGLES);
         for (unsigned int i = 0; i < group->numtriangles; i++) {
             OBJtriangle *triangle = &(model->triangles[group->triangles[i]]);
   
             if (mode & OBJ_FLAT)
                 glNormal3fv(model->facetnorms[triangle->findex]);
   
             if (mode & OBJ_SMOOTH)
                 glNormal3fv(model->normals[triangle->nindices[0]]);
             if (mode & OBJ_TEXTURE)
                 glTexCoord2fv(model->texcoords[triangle->tindices[0]]);
             glVertex3fv(model->vertices[triangle->vindices[0]]);
   
             if (mode & OBJ_SMOOTH)
                 glNormal3fv(model->normals[triangle->nindices[1]]);
             if (mode & OBJ_TEXTURE)
                 glTexCoord2fv(model->texcoords[triangle->tindices[1]]);
             glVertex3fv(model->vertices[triangle->vindices[1]]);
   
             if (mode & OBJ_SMOOTH)
                 glNormal3fv(model->normals[triangle->nindices[2]]);
             if (mode & OBJ_TEXTURE)
                 glTexCoord2fv(model->texcoords[triangle->tindices[2]]);
             glVertex3fv(model->vertices[triangle->vindices[2]]);
         }
         glEnd();
         glEndList();
    }
}

Particle::Particle(Vec3f pos, Vec3f vel, float ttl)
    : pos(pos), vel(vel), ttl(ttl)
{}

bool isDead(Particle *p)
{
    return p->ttl < 0.0;
}

Particle::~Particle()
{}

ParticleSystemInfo::ParticleSystemInfo(std::string filename, Vec3f area, Vec3f velocity, Vec3f velocity_pm, float ttl, float ttl_pm, float birthRate)
    : GeomInfo(), area(area), velocity(velocity), velocity_pm(velocity_pm), ttl(ttl), ttl_pm(ttl_pm), birthRate(birthRate), maxParticles(-1)
{
    World &world = World::getInstance();
    Image2D_t *color = LoadImage((world.assetsDir + filename).c_str(), false, RGBA_IMAGE);
    
    /* Initialize the textures */
    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    TexImage(color);
    FreeImage(color);
    lastUpdate = GetTime();
}

ParticleSystemInfo::ParticleSystemInfo(std::string filename, Vec3f area, Vec3f velocity, Vec3f velocity_pm, float ttl, float ttl_pm, float birthRate, int maxParticles)
    : GeomInfo(), area(area), velocity(velocity), velocity_pm(velocity_pm), ttl(ttl), ttl_pm(ttl_pm), birthRate(birthRate), maxParticles(maxParticles), linearArea(false)
{
    World &world = World::getInstance();
    Image2D_t *color = LoadImage((world.assetsDir + filename).c_str(), false, RGBA_IMAGE);
    
    /* Initialize the textures */
    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    TexImage(color);
    FreeImage(color);
    lastUpdate = GetTime();
}

/* pos should be the position of the parent wobject */
void ParticleSystemInfo::update(ParticleStreamObject *pso, float dt)
{
    for(std::list<Particle *>::iterator i = particles.begin(); i != particles.end(); i++) {
        (*i)->ttl -= dt;
        if ((*i)->ttl < 0.0)
        {
            Particle *toDestroy = *i;
            i = particles.erase(i);
            delete toDestroy;
        } else {
            (*i)->pos += dt * (*i)->vel; 
        }
    }

    if (maxParticles != 0) {
        float random;
        random = (float) rand() / (float) RAND_MAX;
        int toSpawn = lrint(birthRate * (dt / random));

        if (toSpawn > maxParticles && maxParticles > 0)
            toSpawn = maxParticles;
        
        maxParticles -= toSpawn;
        for (int i = 0; i < toSpawn; i++) {
            /* setup the particle including randomization */
            Vec3f p_pos = Vec3f(0.0f, 0.0f, 0.0f);
            if (!linearArea)
            {
                p_pos += randomVec3f(area);
                p_pos.normalize();
                p_pos *= area.length();
            }
            else
            {
                random = (float)rand() / (float) RAND_MAX;
                float dir = atan2(area[2], area[0]);
                random *= area.length();
                if (abs(dir) > M_PI_2)
                    random *= -1;
                p_pos[0] += random;
                p_pos[2] += random * area[2] / area[0];
            }

            if (pso->parent)
                p_pos += pso->parent->getPos();
            else
                p_pos += pso->getPos();

            Vec3f p_vel = velocity;
            p_vel += randomVec3f(velocity_pm);
            p_vel.normalize();
            p_vel *= velocity_pm.length();

            float p_ttl = ttl;
            p_ttl += ((float) rand() / (float) RAND_MAX - 0.5f) * 2 * ttl_pm;

            Particle *particle = new Particle(p_pos, p_vel, p_ttl);

            particles.push_back(particle);
        }
    }
    lastUpdate += dt;
}

void makeExplosion(Vec3f position, float size)
{
    Vec3f area = Vec3f(.01, .01, .01);
    Vec3f velocity = Vec3f(0.0, 0.0, 0.0);
    Vec3f velocity_pm1 = Vec3f(4.5, .1, 4.5);
    Vec3f velocity_pm2 = Vec3f(.1, 4.5, 4.5);
    Vec3f velocity_pm3 = Vec3f(4.5, 4.5, .1);
    velocity_pm1 *= size;
    velocity_pm2 *= size;
    velocity_pm3 *= size;
    float ttl = 1.2;
    float ttl_pm = 0.0;
    float birthRate = 500;
    int maxParticles = 1000;

    ParticleSystemInfo *particleSystem = new ParticleSystemInfo("particles/fire.png", area, velocity, velocity_pm1, ttl, ttl_pm, birthRate, maxParticles);
    GParticleObject *particle_gobj = new GParticleObject(particleSystem);
    ParticleStreamObject *particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL, 3);
    particle_wobj->setPos(position);

    World::getInstance().addObject(particle_wobj);


    particleSystem = new ParticleSystemInfo("particles/fire.png", area, velocity, velocity_pm2, ttl, ttl_pm, birthRate, maxParticles);
    particle_gobj = new GParticleObject(particleSystem);
    particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL, 3);
    particle_wobj->setPos(position);

    World::getInstance().addObject(particle_wobj);

    particleSystem = new ParticleSystemInfo("particles/fire.png", area, velocity, velocity_pm3, ttl, ttl_pm, birthRate, maxParticles);
    particle_gobj = new GParticleObject(particleSystem);
    particle_wobj = new ParticleStreamObject(NULL, particle_gobj, NULL, NULL, 3);
    particle_wobj->setPos(position);

    World::getInstance().addObject(particle_wobj);
}

SkyBoxInfo::SkyBoxInfo(std::string filename)
{
    int i;

    World &world = World::getInstance();
    glGenTextures(NUM_DIR, texID);

    Image2D_t *image;
    for (i = 0; i < NUM_DIR; i++) {
        switch(i) {
            case(NORTH):
                image = LoadImage((world.assetsDir + filename + std::string("north.png")).c_str(), false, RGB_IMAGE);
                break;

            case(EAST):
                image = LoadImage((world.assetsDir + filename + std::string("east.png")).c_str(), false, RGB_IMAGE);
                break;

            case(SOUTH):
                image = LoadImage((world.assetsDir + filename + std::string("south.png")).c_str(), false, RGB_IMAGE);
                break;

            case(WEST):
                image = LoadImage((world.assetsDir + filename + std::string("west.png")).c_str(), false, RGB_IMAGE);
                break;

            case(UP):
                image = LoadImage((world.assetsDir + filename + std::string("up.png")).c_str(), false, RGB_IMAGE);
                break;

            case(DOWN):
                image = LoadImage((world.assetsDir + filename + std::string("down.png")).c_str(), false, RGB_IMAGE);
                break;
        }

        glBindTexture(GL_TEXTURE_2D, texID[i]);
        TexImage(image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        FreeImage(image);
    }
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

dGeomID SkyBoxInfo::createGeom(dSpaceID space)
{ return dCreateSphere (space, 1.0); }

dGeomID  ParticleSystemInfo::createGeom(dSpaceID space)
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

void SkyBoxInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, 1.0, 1.0, 1.0);
}

void ParticleSystemInfo::createMass(dMass * mass, float massVal)
{
    dMassSetBoxTotal(mass, massVal, 1.0, 1.0, 1.0);
}



void SphereInfo::hton(RPGeomInfo *payload) {
    if (payload == NULL){
        cerr << "Payload is null!" << endl;
        return;
    }
    payload->radius = htonf(radius);
    payload->type = htonl(SPHERE);
}

void BoxInfo::hton(RPGeomInfo *payload) {
    if (payload == NULL){
        cerr << "Payload is null!" << endl;
        return;
    }
    payload->lx = htonf(lx);
    payload->ly = htonf(ly);
    payload->lz = htonf(lz);
    payload->type = htonl(BOX);
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
    payload->type = htonf(PLANE);
}

void SphereInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->radius = ntohf(radius);
}

void BoxInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->lx = ntohf(lx);
    payload->ly = ntohf(ly);
    payload->lz = ntohf(lz);
}

void PlaneInfo::ntoh(RPGeomInfo *payload) {
    if (payload == NULL) {
        cerr << "Payload is null!" << endl;
    }
    payload->a = ntohf(a);
    payload->b = ntohf(b);
    payload->c = ntohf(c);
    payload->d = ntohf(d);
}

ObjMeshInfo::~ObjMeshInfo()
{
    OBJDelete(model);
}
