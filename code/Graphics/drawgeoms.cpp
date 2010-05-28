#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include "Engine/geominfo.h"
#include "Engine/world.h"
#include "Utilities/vector.h"
#include <string>
extern "C" {
    #include "Parser/obj-reader.h"
    #include "shader.h"
    #include "Utilities/load-png.h"
}

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
    glVertex3f (-x,-y,-z);
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
{
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_2D);
    
    /* draw the mesh */
    static uint32_t i;
    static OBJgroup* group;
    static OBJtriangle* triangle;
    static OBJmaterial* material;

    assert(model);
    assert(model->vertices);

    uint32_t mode = OBJ_SMOOTH|OBJ_MATERIAL|OBJ_TEXTURE;

    /* do a bit of warning */
    if (mode & OBJ_FLAT && !model->facetnorms) {
        printf("OBJDraw() warning: flat render mode requested "
            "with no facet normals defined.\n");
        mode &= ~OBJ_FLAT;
    }
    if (mode & OBJ_SMOOTH && !model->normals) {
        printf("OBJDraw() warning: smooth render mode requested "
            "with no normals defined.\n");
        mode &= ~OBJ_SMOOTH;
    }
    if (mode & OBJ_TEXTURE && !model->texcoords) {
        printf("OBJDraw() warning: texture render mode requested "
            "with no texture coordinates defined.\n");
        mode &= ~OBJ_TEXTURE;
    }
    if (mode & OBJ_FLAT && mode & OBJ_SMOOTH) {
        printf("OBJDraw() warning: flat render mode requested "
            "and smooth render mode requested (using smooth).\n");
        mode &= ~OBJ_FLAT;
    }
    if (mode & OBJ_COLOR && !model->materials) {
        printf("OBJDraw() warning: color render mode requested "
            "with no materials defined.\n");
        mode &= ~OBJ_COLOR;
    }
    if (mode & OBJ_MATERIAL && !model->materials) {
        printf("OBJDraw() warning: material render mode requested "
            "with no materials defined.\n");
        mode &= ~OBJ_MATERIAL;
    }
    if (mode & OBJ_COLOR && mode & OBJ_MATERIAL) {
        printf("OBJDraw() warning: color and material render mode requested "
            "using only material mode.\n");
        mode &= ~OBJ_COLOR;
    }
    if (mode & OBJ_COLOR)
        glEnable(GL_COLOR_MATERIAL);
    else if (mode & OBJ_MATERIAL)
        glDisable(GL_COLOR_MATERIAL);

    /* perhaps this loop should be unrolled into material, color, flat,
       smooth, etc. loops?  since most cpu's have good branch prediction
       schemes (and these branches will always go one way), probably
       wouldn't gain too much?  */

    group = model->groups;
    while (group) {
        if (mode & OBJ_MATERIAL) {
            material = &model->materials[group->material];
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
        }

        if (mode & OBJ_COLOR) {
            glColor3fv(material->diffuse);
        }

        glBegin(GL_TRIANGLES);
        for (i = 0; i < group->numtriangles; i++) {
            triangle = &(model->triangles[group->triangles[i]]);

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

        group = group->next;
    }

    glDisable(GL_TEXTURE_2D);
}

void SkyBoxInfo::draw()
{

    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0,1.0f);

    // Save Current Matrix
    /* glPushMatrix(); */

    /* glScalef(100.0f, 100.0f, 100.0f); */

    float r = 500.0;

		/* Up (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[UP]);
    glBegin(GL_QUADS);	
    glTexCoord2f(0.0f, 0.0f); glVertex3f(r, r, -r);
    glTexCoord2f(0.0f,  1.0f); glVertex3f(r, r, r); 
    glTexCoord2f(1.0f,  1.0f); glVertex3f(-r, r, r);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-r, r, -r);
    glEnd();

		/* Down (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[DOWN]);
    glBegin(GL_QUADS);		
    glTexCoord2f(1.0f,0.0f);  glVertex3f(-r, -r, -r);
    glTexCoord2f(1.0f,1.0f);  glVertex3f(-r, -r, r);
    glTexCoord2f(0.0f,1.0f);  glVertex3f( r, -r, r); 
    glTexCoord2f(0.0f,0.0f);  glVertex3f( r, -r, -r);
    glEnd();

		/* Right (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[WEST]);
    glBegin(GL_QUADS);		
    glTexCoord2f(0.0f,0.0f); glVertex3f(-r, -r,-r);		
    glTexCoord2f(0.0f,1.0f); glVertex3f(-r, r, -r);
    glTexCoord2f(1.0f,1.0f); glVertex3f(-r, r, r); 
    glTexCoord2f(1.0f,0.0f); glVertex3f(-r, -r, r);	
    glEnd();

		/* Left (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[EAST]);
    glBegin(GL_QUADS);		
    glTexCoord2f(0.0f,0.0f); glVertex3f(r, -r, r);	
    glTexCoord2f(0.0f, 1.0f); glVertex3f(r, r, r); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(r, r, -r);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(r, -r, -r);
    glEnd();

		/* Front (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[NORTH]);
    glBegin(GL_QUADS);		
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-r, -r, r);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-r, r, r);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( r, r, r); 
    glTexCoord2f(1.0f, 0.0f); glVertex3f( r, -r, r);
    glEnd();

		/* Back (from racer start position) */
    glBindTexture(GL_TEXTURE_2D,texID[SOUTH]);
    glBegin(GL_QUADS);		
    glTexCoord2f(0.0f,0.0f);  glVertex3f( r, -r, -r);
    glTexCoord2f(0.0f,1.0f); glVertex3f( r, r, -r); 
    glTexCoord2f(1.0f,1.0f); glVertex3f(-r, r, -r);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-r, -r, -r);
    glEnd();

    // Load Saved Matrix
    /* glPopMatrix(); */

    glDisable(GL_TEXTURE_2D);
}

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

void ParticleSystemInfo::draw()
{
    /* The particles' position are stored in world coordinates, so pop off
     * the matrix with the object coordinate transform. Remember to push a
     * new matrix at the end to prevent too much popping. */
    glPopMatrix();
    glColor3f(0.0, 1.0, 0.0);
    glEnable(GL_TEXTURE_2D);

    glPointSize(5);
    glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
    glEnable( GL_POINT_SPRITE_ARB );

    glBindTexture(GL_TEXTURE_2D, texid);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.3);
    glBegin(GL_POINTS);
    for(std::list<Particle *>::iterator i = particles.begin(); i != particles.end(); i++) {
        glVertex3f((*i)->pos.x, (*i)->pos.y, (*i)->pos.z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POINT_SPRITE_ARB);
    glPushMatrix();
}
