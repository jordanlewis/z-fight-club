#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include "Engine/geominfo.h"
#include "Utilities/error.h"
#include "Utilities/vector.h"


/* Credit to ODE's drawstuff library */
void BoxInfo::draw()
{
    float x = lx*0.5f;
    float y = ly*0.5f;
    float z = lz*0.5f;

    // sides
    glBegin (GL_TRIANGLE_STRIP);
    glNormal3f (-1,0,0);
    glVertex3f (-x,-y,-z);
    glVertex3f (-x,-y,z);
    glVertex3f (-x,y,-z);
    glVertex3f (-x,y,z);
    glNormal3f (0,1,0);
    glVertex3f (x,y,-z);
    glVertex3f (x,y,z);
    glNormal3f (1,0,0);
    glVertex3f (x,-y,-z);
    glVertex3f (x,-y,z);
    glNormal3f (0,-1,0);
    glVertex3f (-x,-y,-lz);
    glVertex3f (-x,-y,z);
    glEnd();

    // top face
    glBegin (GL_TRIANGLE_FAN);
    glNormal3f (0,0,1);
    glVertex3f (-x,-y,z);
    glVertex3f (x,-y,z);
    glVertex3f (x,y,z);
    glVertex3f (-x,y,z);
    glEnd();

    // bottom face
    glBegin (GL_TRIANGLE_FAN);
    glNormal3f (0,0,-1);
    glVertex3f (-x,-y,-z);
    glVertex3f (-x,y,-z);
    glVertex3f (x,y,-z);
    glVertex3f (x,-y,-z);
    glEnd();
}

void ObjMeshInfo::draw()
{}

void TriMeshInfo::draw()
{
    if (normals == NULL)
    {
        normals = new Vec3f_t[nTris];
        Vec3f_t tmp1, tmp2;
        for (int i = 0; i < nTris / 3; i++)
        {
            SubV3f(verts[tris[i * 3 + 1]], verts[tris[i * 3]], tmp1);
            SubV3f(verts[tris[i * 3 + 2]], verts[tris[i * 3]], tmp2);
            CrossV3f(tmp1, tmp2, tmp1);
            memcpy(normals + i * 3, tmp1, sizeof(float) * 3);
            memcpy(normals + i * 3 + 1, tmp1, sizeof(float) * 3);
            memcpy(normals + i * 3 + 2, tmp1, sizeof(float) * 3);
        }
    }
    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, verts);
    glNormalPointer(GL_FLOAT, 0, normals);
    glDrawElements(GL_TRIANGLES, nTris, GL_UNSIGNED_INT, tris);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_CULL_FACE);
}

void SphereInfo::draw()
{
    GLUquadric * quad = gluNewQuadric();
    gluSphere(quad, radius, 20, 20);
}
