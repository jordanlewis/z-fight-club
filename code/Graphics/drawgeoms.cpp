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
{
    /* we should do some sort of caching to make this faster */
    int i, j;

    /* get vertex data ready to load into opengl */
    float *vert = new float[obj.vertexCount * 3];
    float *normal = new float[obj.normalCount * 3];
    float *uv = new float[obj.textureCount * 2];

    for (i = 0; i < obj.vertexCount; i++)
	for (j = 0; j < 3; j++)
	    vert[(3 * i) + j] = obj.vertexList[i]->e[j];

    for (i = 0; i < obj.normalCount; i++)
	for (j = 0; j < 3; j++)
	    normal[(3*i) + j] = obj.normalList[i]->e[j];

    for (i = 0; i < obj.textureCount; i++)
	for (j = 0; j < 2; j++)
	    uv[(2*i) + j] = obj.textureList[i]->e[j];

    /* get face indices ready to load in to data */
    uint32_t *tri = new uint32_t[obj.faceCount * 3];
    uint32_t *triNormal = new uint32_t[obj.faceCount * 3];
    uint32_t *triUv = new uint32_t[obj.faceCount * 3];

    uint32_t *quad = new uint32_t[obj.faceCount * 4];
    uint32_t *quadNormal = new uint32_t[obj.faceCount * 4];
    uint32_t *quadUv = new uint32_t[obj.faceCount * 4];

    int nTris = 0;
    int nQuads = 0;

    for (i = 0; i < obj.faceCount; i++) {
	if (obj.faceList[i]->vertex_count == 3) {
	    /* Triangle */

	    for (j = 0; j < 3; j++) {
		tri[nTris * 3 + j] = obj.faceList[i]->vertex_index[j];
		triNormal[nTris * 3 + j] = obj.faceList[i]->normal_index[j];
		triUv[nTris * 3 + j] = obj.faceList[i]->texture_index[j];
	    }

	    nTris++;
	} else if(obj.faceList[i]->vertex_count == 4) {
	    /* Quad */

	    for (j = 0; j < 4; j++) {
		tri[nQuads * 4 + j] = obj.faceList[i]->vertex_index[j];
		triNormal[nQuads * 4 + j] = obj.faceList[i]->normal_index[j];
		triUv[nQuads * 4 + j] = obj.faceList[i]->texture_index[j];
	    }

	    nQuads++;
	} else {
	    Error error = Error::getInstance();
	    error.log(GRAPHICS, CRITICAL, "Malformed, obj exiting\n");
	    exit(1);
	}
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vert);
    glNormalPointer(GL_FLOAT, 0, normal);
    glTexCoordPointer(2, GL_FLOAT, 0, uv);

    glDrawElements(GL_TRIANGLES, nTris * 3, GL_UNSIGNED_INT, tri);
    glDrawElements(GL_QUADS, nQuads * 4, GL_UNSIGNED_INT, quad);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    delete [] vert;
    delete [] normal;
    delete [] uv;

    delete [] tri;
    delete [] triNormal;
    delete [] triUv;

    delete [] quad;
    delete [] quadNormal;
    delete [] quadUv;
}
