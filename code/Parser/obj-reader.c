/*    
      obj-reader.c
      Nate Robins, 1997, 2000
      nate@pobox.com, http://www.pobox.com/~nate
 
      Wavefront OBJ model file format reader/writer/manipulator.

      Includes routines for generating smooth normals with
      preservation of edges, welding redundant vertices & texture
      coordinate generation (spheremap and planar projections) + more.
  
*/

#include "Utilities/defs.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "obj-reader.h"

#define T(x) (model->triangles[(x)])

/* _OBJnode: general purpose node */
typedef struct _OBJnode {
    uint32_t index;
    bool averaged;
    struct _OBJnode* next;
} OBJnode;


/* OBJMax: returns the maximum of two floats */
static inline float OBJMax(float a, float b) 
{
    if (b > a) return b; else return a;
}

/* OBJAbs: returns the absolute value of a float */
static inline float OBJAbs(float f)
{
    if (f < 0) return -f; else return f;
}

/* OBJEqual: compares two vectors and returns true if they are
 * equal (within a certain threshold) or false if not. An epsilon
 * that works fairly well is 0.000001.
 *
 * u - array of 3 GLfloats (float u[3])
 * v - array of 3 GLfloats (float v[3]) 
 */
static bool
OBJEqual(Vec3f_t u, Vec3f_t v, float epsilon)
{
    return (OBJAbs(u[0] - v[0]) < epsilon &&
        OBJAbs(u[1] - v[1]) < epsilon &&
        OBJAbs(u[2] - v[2]) < epsilon);
}

/* OBJWeldVectors: eliminate (weld) vectors that are within an
 * epsilon of each other.
 *
 * vectors     - array of float[3]'s to be welded
 * numvectors - number of float[3]'s in vectors
 * epsilon     - maximum difference between vectors 
 *
 */
Vec3f_t *
OBJWeldVectors(Vec3f_t *vectors, uint32_t* numvectors, float epsilon)
{
    Vec3f_t *copies;
    uint32_t copied;
    uint32_t i, j;
    
    copies = NEWVEC(Vec3f_t, *numvectors + 1);
    memcpy(copies, vectors, (sizeof(Vec3f_t) * (*numvectors + 1)));
    
    copied = 1;
    for (i = 1; i <= *numvectors; i++) {
        for (j = 1; j <= copied; j++) {
            if (OBJEqual(vectors[i], copies[j], epsilon)) {
                goto duplicate;
            }
        }
        
        /* must not be any duplicates -- add to the copies array */
        copies[copied][0] = vectors[i][0];
        copies[copied][1] = vectors[i][1];
        copies[copied][2] = vectors[i][2];
        j = copied;             /* pass this along for below */
        copied++;
        
duplicate:
        /* set the first component of this vector to point at the correct
        index into the new copies array */
        vectors[i][0] = (float)j;
    }
    
    *numvectors = copied-1;
    return copies;
}

/* OBJFindGroup: Find a group in the model */
OBJgroup*
OBJFindGroup(OBJmodel* model, char* name)
{
    OBJgroup* group;
    
    assert(model);
    
    group = model->groups;
    while(group) {
        if (!strcmp(name, group->name))
            break;
        group = group->next;
    }
    
    return group;
}

/* OBJAddGroup: Add a group to the model */
OBJgroup*
OBJAddGroup(OBJmodel* model, char* name)
{
    OBJgroup* group;
    
    group = OBJFindGroup(model, name);
    if (!group) {
        group = NEW(OBJgroup);
        group->name = strdup(name);
        group->material = 0;
        group->numtriangles = 0;
        group->triangles = NULL;
        group->next = model->groups;
        model->groups = group;
        model->numgroups++;
    }
    
    return group;
}

/* OBJFindGroup: Find a material in the model */
uint32_t
OBJFindMaterial(OBJmodel* model, char* name)
{
    uint32_t i;
    
    /* XXX doing a linear search on a string key'd list is pretty lame,
    but it works and is fast enough for now. */
    for (i = 0; i < model->nummaterials; i++) {
        if (!strcmp(model->materials[i].name, name))
            goto found;
    }
    
    /* didn't find the name, so print a warning and return the default
    material (0). */
    printf("OBJFindMaterial():  can't find material \"%s\".\n", name);
    i = 0;
    
found:
    return i;
}


/* OBJDirName: return the directory given a path
 *
 * path - filesystem path
 *
 * NOTE: the return value should be free'd.
 */
static char*
OBJDirName(char* path)
{
    char* dir;
    char* s;
    
    dir = strdup(path);
    
    s = strrchr(dir, '/');
    if (s)
        s[1] = '\0';
    else
        dir[0] = '\0';
    
    return dir;
}


/* OBJReadMTL: read a wavefront material library file
 *
 * model - properly initialized OBJmodel structure
 * name  - name of the material library
 */
static void
OBJReadMTL(OBJmodel* model, const char* name)
{
    FILE* file;
    char* dir;
    char* filename;
    char buf[128];
    uint32_t nummaterials, i;
    
    dir = OBJDirName(model->pathname);
    filename = NEWVEC(char, strlen(dir) + strlen(name) + 1);
    strcpy(filename, dir);
    strcat(filename, name);
    FREE(dir);
    
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "OBJReadMTL() failed: can't open material file \"%s\".\n",
            filename);
        exit(1);
    }
    FREE(filename);
    
    /* count the number of materials in the file */
    nummaterials = 1;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'n':               /* newmtl */
            fgets(buf, sizeof(buf), file);
            nummaterials++;
            sscanf(buf, "%s %s", buf, buf);
            break;
        default:
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        }
    }
    
    rewind(file);
    
    model->materials = NEWVEC(OBJmaterial, nummaterials);
    model->nummaterials = nummaterials;
    
    /* set the default material */
    for (i = 0; i < nummaterials; i++) {
        model->materials[i].name = NULL;
        model->materials[i].shininess = 65.0;
        model->materials[i].diffuse[0] = 0.8;
        model->materials[i].diffuse[1] = 0.8;
        model->materials[i].diffuse[2] = 0.8;
        model->materials[i].diffuse[3] = 1.0;
        model->materials[i].ambient[0] = 0.2;
        model->materials[i].ambient[1] = 0.2;
        model->materials[i].ambient[2] = 0.2;
        model->materials[i].ambient[3] = 1.0;
        model->materials[i].specular[0] = 0.0;
        model->materials[i].specular[1] = 0.0;
        model->materials[i].specular[2] = 0.0;
        model->materials[i].specular[3] = 1.0;
    }
    model->materials[0].name = strdup("default");
    
    /* now, read in the data */
    nummaterials = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'n':               /* newmtl */
            fgets(buf, sizeof(buf), file);
            sscanf(buf, "%s %s", buf, buf);
            nummaterials++;
            model->materials[nummaterials].name = strdup(buf);
            break;
        case 'N':
            fscanf(file, "%f", &model->materials[nummaterials].shininess);
            /* wavefront shininess is from [0, 1000], so scale for OpenGL */
            model->materials[nummaterials].shininess /= 1000.0;
            model->materials[nummaterials].shininess *= 128.0;
            break;
        case 'K':
            switch(buf[1]) {
            case 'd':
                fscanf(file, "%f %f %f",
                    &model->materials[nummaterials].diffuse[0],
                    &model->materials[nummaterials].diffuse[1],
                    &model->materials[nummaterials].diffuse[2]);
                break;
            case 's':
                fscanf(file, "%f %f %f",
                    &model->materials[nummaterials].specular[0],
                    &model->materials[nummaterials].specular[1],
                    &model->materials[nummaterials].specular[2]);
                break;
            case 'a':
                fscanf(file, "%f %f %f",
                    &model->materials[nummaterials].ambient[0],
                    &model->materials[nummaterials].ambient[1],
                    &model->materials[nummaterials].ambient[2]);
                break;
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            }
            break;
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
    }

    fclose(file);
}

/* OBJWriteMTL: write a wavefront material library file
 *
 * model   - properly initialized OBJmodel structure
 * modelpath  - pathname of the model being written
 * mtllibname - name of the material library to be written
 */
static void
OBJWriteMTL(OBJmodel* model, char* modelpath, char* mtllibname)
{
    FILE* file;
    char* dir;
    char* filename;
    OBJmaterial* material;
    uint32_t i;
    
    dir = OBJDirName(modelpath);
    filename = NEWVEC(char, strlen(dir)+strlen(mtllibname));
    strcpy(filename, dir);
    strcat(filename, mtllibname);
    FREE(dir);
    
    /* open the file */
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "OBJWriteMTL() failed: can't open file \"%s\".\n",
            filename);
        exit(1);
    }
    FREE(filename);
    
    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront MTL generated by OBJ library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  OBJ library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n\n");
    
    for (i = 0; i < model->nummaterials; i++) {
        material = &model->materials[i];
        fprintf(file, "newmtl %s\n", material->name);
        fprintf(file, "Ka %f %f %f\n", 
            material->ambient[0], material->ambient[1], material->ambient[2]);
        fprintf(file, "Kd %f %f %f\n", 
            material->diffuse[0], material->diffuse[1], material->diffuse[2]);
        fprintf(file, "Ks %f %f %f\n", 
            material->specular[0],material->specular[1],material->specular[2]);
        fprintf(file, "Ns %f\n", material->shininess / 128.0 * 1000.0);
        fprintf(file, "\n");
    }

    fclose(file);
}


/* OBJFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertices, #normals, etc)
 *
 * model - properly initialized OBJmodel structure
 * file  - (fopen'd) file descriptor 
 */
static void
OBJFirstPass(OBJmodel* model, FILE* file) 
{
    uint32_t numvertices;        /* number of vertices in model */
    uint32_t numnormals;         /* number of normals in model */
    uint32_t numtexcoords;       /* number of texcoords in model */
    uint32_t numtriangles;       /* number of triangles in model */
    OBJgroup* group;           /* current group */
    int v, n, t;
    char buf[128];
    
    /* make a default group */
    group = OBJAddGroup(model, "default");
    
    numvertices = numnormals = numtexcoords = numtriangles = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numvertices++;
                break;
            case 'n':           /* normal */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numnormals++;
                break;
            case 't':           /* texcoord */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numtexcoords++;
                break;
            default:
                printf("OBJFirstPass(): Unknown token \"%s\".\n", buf);
                exit(1);
                break;
            }
            break;
            case 'm':
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                model->mtllibname = strdup(buf);
                OBJReadMTL(model, buf);
                break;
            case 'u':
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = OBJAddGroup(model, buf);
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    fscanf(file, "%d/%d", &v, &t);
                    fscanf(file, "%d/%d", &v, &t);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else {
                    /* v */
                    fscanf(file, "%d", &v);
                    fscanf(file, "%d", &v);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
  }
  
  /* set the stats in the model structure */
  model->numvertices  = numvertices;
  model->numnormals   = numnormals;
  model->numtexcoords = numtexcoords;
  model->numtriangles = numtriangles;
  
  /* allocate memory for the triangles in each group */
  group = model->groups;
  while(group) {
      group->triangles = NEWVEC(uint32_t, group->numtriangles);
      group->numtriangles = 0;
      group = group->next;
  }
}

/* OBJSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized OBJmodel structure
 * file  - (fopen'd) file descriptor 
 */
static void
OBJSecondPass(OBJmodel* model, FILE* file) 
{
    uint32_t numvertices;	/* number of vertices in model */
    uint32_t numnormals;	/* number of normals in model */
    uint32_t numtexcoords;	/* number of texcoords in model */
    uint32_t numtriangles;	/* number of triangles in model */
    Vec3f_t *vertices;		/* array of vertices  */
    Vec3f_t *normals;		/* array of normals */
    Vec2f_t *texcoords;		/* array of texture coordinates */
    OBJgroup* group;		/* current group pointer */
    uint32_t material;		/* current material */
    int v, n, t;
    char buf[128];
    
    /* set the pointer shortcuts */
    vertices	= model->vertices;
    normals	= model->normals;
    texcoords	= model->texcoords;
    group	= model->groups;
    
    /* on the second pass through the file, read all the data into the
    allocated arrays */
    numvertices = numnormals = numtexcoords = 1;
    numtriangles = 0;
    material = 0;
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
                fscanf(file, "%f %f %f", 
                    &(vertices[numvertices][0]), 
                    &(vertices[numvertices][1]), 
                    &(vertices[numvertices][2]));
                numvertices++;
                break;
            case 'n':           /* normal */
                fscanf(file, "%f %f %f", 
                    &(normals[numnormals][0]),
                    &(normals[numnormals][1]), 
                    &(normals[numnormals][2]));
                numnormals++;
                break;
            case 't':           /* texcoord */
                fscanf(file, "%f %f", 
                    &(texcoords[numtexcoords][0]),
                    &(texcoords[numtexcoords][1]));
                numtexcoords++;
                break;
            }
            break;
            case 'u':
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                group->material = material = OBJFindMaterial(model, buf);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = OBJFindGroup(model, buf);
                group->material = material;
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[0] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[1] = n < 0 ? n + numnormals : n;
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                    T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                        T(numtriangles).nindices[2] = n < 0 ? n + numnormals : n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[0] = t < 0 ? t + numtexcoords : t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[1] = t < 0 ? t + numtexcoords : t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        T(numtriangles).tindices[2] = t < 0 ? t + numtexcoords : t;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else {
                    /* v */
                    sscanf(buf, "%d", &v);
                    T(numtriangles).vindices[0] = v < 0 ? v + numvertices : v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[1] = v < 0 ? v + numvertices : v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).vindices[2] = v < 0 ? v + numvertices : v;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
    }
  }
  
#if 0
  /* announce the memory requirements */
  printf(" Memory: %d bytes\n",
      numvertices  * sizeof(Vec3f_t) +
      numnormals   * sizeof(Vec3f_t) * (numnormals ? 1 : 0) +
      numtexcoords * sizeof(Vec3f_t) * (numtexcoords ? 1 : 0) +
      numtriangles * sizeof(OBJtriangle));
#endif
}


/* public functions */


/* OBJUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.   Returns the
 * scalefactor used.
 *
 * model - properly initialized OBJmodel structure 
 */
float
OBJUnitize(OBJmodel* model)
{
    uint32_t i;
    float maxx, minx, maxy, miny, maxz, minz;
    float cx, cy, cz, w, h, d;
    float scale;
    
    assert(model);
    assert(model->vertices);
    
    /* get the max/mins */
    maxx = minx = model->vertices[1][0];
    maxy = miny = model->vertices[1][1];
    maxz = minz = model->vertices[1][2];
    for (i = 2; i <= model->numvertices; i++) {
        if (maxx < model->vertices[i][0])
            maxx = model->vertices[i][0];
        if (minx > model->vertices[i][0])
            minx = model->vertices[i][0];
        
        if (maxy < model->vertices[i][1])
            maxy = model->vertices[i][1];
        if (miny > model->vertices[i][1])
            miny = model->vertices[i][1];
        
        if (maxz < model->vertices[i][2])
            maxz = model->vertices[i][2];
        if (minz > model->vertices[i][2])
            minz = model->vertices[i][2];
    }
    
    /* calculate model width, height, and depth */
    w = OBJAbs(maxx) + OBJAbs(minx);
    h = OBJAbs(maxy) + OBJAbs(miny);
    d = OBJAbs(maxz) + OBJAbs(minz);
    
    /* calculate center of the model */
    cx = (maxx + minx) / 2.0;
    cy = (maxy + miny) / 2.0;
    cz = (maxz + minz) / 2.0;
    
    /* calculate unitizing scale factor */
    scale = 2.0 / OBJMax(OBJMax(w, h), d);
    
    /* translate around center then scale */
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[i][0] -= cx;
        model->vertices[i][1] -= cy;
        model->vertices[i][2] -= cz;
        model->vertices[i][0] *= scale;
        model->vertices[i][1] *= scale;
        model->vertices[i][2] *= scale;
    }
    
    return scale;
}

/* OBJDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model   - initialized OBJmodel structure
 * dimensions - array of 3 GLfloats (float dimensions[3])
 */
void
OBJDimensions(OBJmodel* model, float* dimensions)
{
    uint32_t i;
    float maxx, minx, maxy, miny, maxz, minz;
    
    assert(model);
    assert(model->vertices);
    assert(dimensions);
    
    /* get the max/mins */
    maxx = minx = model->vertices[1][0];
    maxy = miny = model->vertices[1][1];
    maxz = minz = model->vertices[1][2];
    for (i = 2; i <= model->numvertices; i++) {
        if (maxx < model->vertices[i][0])
            maxx = model->vertices[i][0];
        if (minx > model->vertices[i][0])
            minx = model->vertices[i][0];
        
        if (maxy < model->vertices[i][1])
            maxy = model->vertices[i][1];
        if (miny > model->vertices[i][1])
            miny = model->vertices[i][1];
        
        if (maxz < model->vertices[i][2])
            maxz = model->vertices[i][2];
        if (minz > model->vertices[i][2])
            minz = model->vertices[i][2];
    }
    
    /* calculate model width, height, and depth */
    dimensions[0] = OBJAbs(maxx) + OBJAbs(minx);
    dimensions[1] = OBJAbs(maxy) + OBJAbs(miny);
    dimensions[2] = OBJAbs(maxz) + OBJAbs(minz);
}

/* OBJScale: Scales a model by a given amount.
 * 
 * model - properly initialized OBJmodel structure
 * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
 */
void
OBJScale(OBJmodel* model, float scale)
{
    uint32_t i;
    
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[i][0] *= scale;
        model->vertices[i][1] *= scale;
        model->vertices[i][2] *= scale;
    }
}

/* OBJReverseWinding: Reverse the polygon winding for all polygons in
 * this model.   Default winding is counter-clockwise.  Also changes
 * the direction of the normals.
 * 
 * model - properly initialized OBJmodel structure 
 */
void
OBJReverseWinding(OBJmodel* model)
{
    uint32_t i, swap;
    
    assert(model);
    
    for (i = 0; i < model->numtriangles; i++) {
        swap = T(i).vindices[0];
        T(i).vindices[0] = T(i).vindices[2];
        T(i).vindices[2] = swap;
        
        if (model->numnormals) {
            swap = T(i).nindices[0];
            T(i).nindices[0] = T(i).nindices[2];
            T(i).nindices[2] = swap;
        }
        
        if (model->numtexcoords) {
            swap = T(i).tindices[0];
            T(i).tindices[0] = T(i).tindices[2];
            T(i).tindices[2] = swap;
        }
    }
    
    /* reverse facet normals */
    for (i = 1; i <= model->numfacetnorms; i++) {
        model->facetnorms[i][0] = -model->facetnorms[i][0];
        model->facetnorms[i][1] = -model->facetnorms[i][1];
        model->facetnorms[i][2] = -model->facetnorms[i][2];
    }
    
    /* reverse vertex normals */
    for (i = 1; i <= model->numnormals; i++) {
        model->normals[i][0] = -model->normals[i][0];
        model->normals[i][1] = -model->normals[i][1];
        model->normals[i][2] = -model->normals[i][2];
    }
}

/* OBJFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized OBJmodel structure
 */
void
OBJFacetNormals(OBJmodel* model)
{
    uint32_t  i;
    float u[3];
    float v[3];
    
    assert(model);
    assert(model->vertices);
    
    /* clobber any old facetnormals */
    if (model->facetnorms)
        FREE(model->facetnorms);
    
    /* allocate memory for the new facet normals */
    model->numfacetnorms = model->numtriangles;
    model->facetnorms = NEWVEC(Vec3f_t, model->numfacetnorms + 1);
    
    for (i = 0; i < model->numtriangles; i++) {
        model->triangles[i].findex = i+1;
        
        u[0] = model->vertices[3 * T(i).vindices[1] + 0] -
            model->vertices[3 * T(i).vindices[0] + 0];
        u[1] = model->vertices[3 * T(i).vindices[1] + 1] -
            model->vertices[3 * T(i).vindices[0] + 1];
        u[2] = model->vertices[3 * T(i).vindices[1] + 2] -
            model->vertices[3 * T(i).vindices[0] + 2];
        
        v[0] = model->vertices[3 * T(i).vindices[2] + 0] -
            model->vertices[3 * T(i).vindices[0] + 0];
        v[1] = model->vertices[3 * T(i).vindices[2] + 1] -
            model->vertices[3 * T(i).vindices[0] + 1];
        v[2] = model->vertices[3 * T(i).vindices[2] + 2] -
            model->vertices[3 * T(i).vindices[0] + 2];
        
        CrossV3f(u, v, model->facetnorms[i+1]);
        NormalizeV3f(model->facetnorms[i+1]);
    }
}

/* OBJVertexNormals: Generates smooth vertex normals for a model.
 * First builds a list of all the triangles each vertex is in.   Then
 * loops through each vertex in the the list averaging all the facet
 * normals of the triangles each vertex is in.   Finally, sets the
 * normal index in the triangle for the vertex to the generated smooth
 * normal.   If the dot product of a facet normal and the facet normal
 * associated with the first triangle in the list of triangles the
 * current vertex is in is greater than the cosine of the angle
 * parameter to the function, that facet normal is not added into the
 * average normal calculation and the corresponding vertex is given
 * the facet normal.  This tends to preserve hard edges.  The angle to
 * use depends on the model, but 90 degrees is usually a good start.
 *
 * model - initialized OBJmodel structure
 * angle - maximum angle (in degrees) to smooth across
 */
void
OBJVertexNormals(OBJmodel* model, float angle)
{
    OBJnode* node;
    OBJnode* tail;
    OBJnode** members;
    Vec3f_t *normals;
    uint32_t numnormals;
    float average[3];
    float dot, cos_angle;
    uint32_t i, avg;
    
    assert(model);
    assert(model->facetnorms);
    
    /* calculate the cosine of the angle (in degrees) */
    cos_angle = cos(angle * M_PI / 180.0);
    
    /* nuke any previous normals */
    if (model->normals)
        FREE(model->normals);
    
    /* allocate space for new normals */
    model->numnormals = model->numtriangles * 3; /* 3 normals per triangle */
    model->normals = NEWVEC(Vec3f_t, model->numnormals+1);
    
    /* allocate a structure that will hold a linked list of triangle
    indices for each vertex */
    members = NEWVEC(OBJnode *, model->numvertices + 1);
    for (i = 1; i <= model->numvertices; i++)
        members[i] = NULL;
    
    /* for every triangle, create a node for each vertex in it */
    for (i = 0; i < model->numtriangles; i++) {
        node = NEW(OBJnode);
        node->index = i;
        node->next  = members[T(i).vindices[0]];
        members[T(i).vindices[0]] = node;
        
        node = NEW(OBJnode);
        node->index = i;
        node->next  = members[T(i).vindices[1]];
        members[T(i).vindices[1]] = node;
        
        node = NEW(OBJnode);
        node->index = i;
        node->next  = members[T(i).vindices[2]];
        members[T(i).vindices[2]] = node;
    }
    
    /* calculate the average normal for each vertex */
    numnormals = 1;
    for (i = 1; i <= model->numvertices; i++) {
    /* calculate an average normal for this vertex by averaging the
        facet normal of every triangle this vertex is in */
        node = members[i];
        if (!node)
            fprintf(stderr, "OBJVertexNormals(): vertex w/o a triangle\n");
        average[0] = 0.0; average[1] = 0.0; average[2] = 0.0;
        avg = 0;
        while (node) {
        /* only average if the dot product of the angle between the two
        facet normals is greater than the cosine of the threshold
        angle -- or, said another way, the angle between the two
            facet normals is less than (or equal to) the threshold angle */
            dot = DotV3f(model->facetnorms[T(node->index).findex],
                model->facetnorms[T(members[i]->index).findex]);
            if (dot > cos_angle) {
                node->averaged = true;
		AddV3f(average, model->facetnorms[T(node->index).findex], average);
                avg = 1;            /* we averaged at least one normal! */
            } else {
                node->averaged = false;
            }
            node = node->next;
        }
        
        if (avg > 0) {
            /* normalize the averaged normal */
            NormalizeV3f(average);
            
            /* add the normal to the vertex normals list */
	    CopyV3f (average, model->normals[3 * numnormals]);
            avg = numnormals;
            numnormals++;
        }
        
        /* set the normal of this vertex in each triangle it is in */
        node = members[i];
        while (node) {
            if (node->averaged) {
                /* if this node was averaged, use the average normal */
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = avg;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = avg;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = avg;
            } else {
                /* if this node wasn't averaged, use the facet normal */
		CopyV3f(model->facetnorms[3 * T(node->index).findex], model->normals[3 * numnormals]);
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = numnormals;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = numnormals;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = numnormals;
                numnormals++;
            }
            node = node->next;
        }
    }
    
    model->numnormals = numnormals - 1;
    
    /* free the member information */
    for (i = 1; i <= model->numvertices; i++) {
        node = members[i];
        while (node) {
            tail = node;
            node = node->next;
            FREE(tail);
        }
    }
    FREE(members);
    
    /* pack the normals array (we previously allocated the maximum
    number of normals that could possibly be created (numtriangles *
    3), so get rid of some of them (usually alot unless none of the
    facet normals were averaged)) */
    normals = model->normals;
    model->normals = NEWVEC(Vec3f_t, 3 * (model->numnormals+1));
    for (i = 1; i <= model->numnormals; i++) {
	CopyV3f (normals[3 * i], model->normals[3 * i]);
    }
    FREE(normals);
}


/* OBJLinearTexture: Generates texture coordinates according to a
 * linear projection of the texture map.  It generates these by
 * linearly mapping the vertices onto a square.
 *
 * model - pointer to initialized OBJmodel structure
 */
void
OBJLinearTexture(OBJmodel* model)
{
    OBJgroup *group;
    float dimensions[3];
    float x, y, scalefactor;
    uint32_t i;
    
    assert(model);
    
    if (model->texcoords)
        FREE(model->texcoords);
    model->numtexcoords = model->numvertices;
    model->texcoords = NEWVEC(Vec2f_t, model->numtexcoords+1);
    
    OBJDimensions(model, dimensions);
    scalefactor = 2.0 / 
        OBJAbs(OBJMax(OBJMax(dimensions[0], dimensions[1]), dimensions[2]));
    
    /* do the calculations */
    for(i = 1; i <= model->numvertices; i++) {
        x = model->vertices[i][0] * scalefactor;
        y = model->vertices[i][2] * scalefactor;
        model->texcoords[i][0] = (x + 1.0) / 2.0;
        model->texcoords[i][1] = (y + 1.0) / 2.0;
    }
    
    /* go through and put texture coordinate indices in all the triangles */
    group = model->groups;
    while(group) {
        for(i = 0; i < group->numtriangles; i++) {
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).vindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).vindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).vindices[2];
        }    
        group = group->next;
    }
    
#if 0
    printf("OBJLinearTexture(): generated %d linear texture coordinates\n",
        model->numtexcoords);
#endif
}

/* OBJSpheremapTexture: Generates texture coordinates according to a
 * spherical projection of the texture map.  Sometimes referred to as
 * spheremap, or reflection map texture coordinates.  It generates
 * these by using the normal to calculate where that vertex would map
 * onto a sphere.  Since it is impossible to map something flat
 * perfectly onto something spherical, there is distortion at the
 * poles.  This particular implementation causes the poles along the X
 * axis to be distorted.
 *
 * model - pointer to initialized OBJmodel structure
 */
void
OBJSpheremapTexture(OBJmodel* model)
{
    OBJgroup* group;
    float theta, phi, rho, x, y, z, r;
    uint32_t i;
    
    assert(model);
    assert(model->normals);
    
    if (model->texcoords)
        FREE(model->texcoords);
    model->numtexcoords = model->numnormals;
    model->texcoords=NEWVEC(Vec2f_t, model->numtexcoords+1);
    
    for (i = 1; i <= model->numnormals; i++) {
        z = model->normals[i][0];  /* re-arrange for pole distortion */
        y = model->normals[i][1];
        x = model->normals[i][2];
        r = sqrt((x * x) + (y * y));
        rho = sqrt((r * r) + (z * z));
        
        if(r == 0.0) {
            theta = 0.0;
            phi = 0.0;
        } else {
            if(z == 0.0)
                phi = 3.14159265 / 2.0;
            else
                phi = acos(z / rho);
            
            if(y == 0.0)
                theta = 3.141592365 / 2.0;
            else
                theta = asin(y / r) + (3.14159265 / 2.0);
        }
        
        model->texcoords[i][0] = theta / 3.14159265;
        model->texcoords[i][1] = phi / 3.14159265;
    }
    
    /* go through and put texcoord indices in all the triangles */
    group = model->groups;
    while(group) {
        for (i = 0; i < group->numtriangles; i++) {
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).nindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).nindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).nindices[2];
        }
        group = group->next;
    }
}

/* OBJDelete: Deletes a OBJmodel structure.
 *
 * model - initialized OBJmodel structure
 */
void
OBJDelete(OBJmodel* model)
{
    OBJgroup* group;
    uint32_t i;
    
    assert(model);
    
    if (model->pathname)     FREE(model->pathname);
    if (model->mtllibname) FREE(model->mtllibname);
    if (model->vertices)     FREE(model->vertices);
    if (model->normals)  FREE(model->normals);
    if (model->texcoords)  FREE(model->texcoords);
    if (model->facetnorms) FREE(model->facetnorms);
    if (model->triangles)  FREE(model->triangles);
    if (model->materials) {
        for (i = 0; i < model->nummaterials; i++)
            FREE(model->materials[i].name);
    }
    FREE(model->materials);
    while(model->groups) {
        group = model->groups;
        model->groups = model->groups->next;
        FREE(group->name);
        FREE(group->triangles);
        FREE(group);
    }
    
    FREE(model);
}

/* OBJReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * OBJDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.  
 */
OBJmodel* 
OBJReadOBJ(const char* filename)
{
    OBJmodel* model;
    FILE* file;
    
    /* open the file */
    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "OBJReadOBJ() failed: can't open data file \"%s\".\n",
            filename);
        exit(1);
    }
    
    /* allocate a new model */
    model = NEW(OBJmodel);
    model->pathname    = strdup(filename);
    model->mtllibname    = NULL;
    model->numvertices   = 0;
    model->vertices    = NULL;
    model->numnormals    = 0;
    model->normals     = NULL;
    model->numtexcoords  = 0;
    model->texcoords       = NULL;
    model->numfacetnorms = 0;
    model->facetnorms    = NULL;
    model->numtriangles  = 0;
    model->triangles       = NULL;
    model->nummaterials  = 0;
    model->materials       = NULL;
    model->numgroups       = 0;
    model->groups      = NULL;
    model->position[0]   = 0.0;
    model->position[1]   = 0.0;
    model->position[2]   = 0.0;
    
    /* make a first pass through the file to get a count of the number
    of vertices, normals, texcoords & triangles */
    OBJFirstPass(model, file);
    
    /* allocate memory */
    model->vertices = NEWVEC(Vec3f_t, model->numvertices + 1);
    model->triangles = NEWVEC(OBJtriangle, model->numtriangles);
    if (model->numnormals > 0) {
        model->normals = NEWVEC(Vec3f_t,model->numnormals + 1);
    }
    if (model->numtexcoords > 0) {
        model->texcoords = NEWVEC(Vec2f_t, model->numtexcoords + 1);
    }
    
    /* rewind to beginning of file and read in the data this pass */
    rewind(file);
    
    OBJSecondPass(model, file);
    
    /* close the file */
    fclose(file);
    
    return model;
}

/* OBJWriteOBJ: Writes a model description in Wavefront .OBJ format to
 * a file.
 *
 * model - initialized OBJmodel structure
 * filename - name of the file to write the Wavefront .OBJ format data to
 * mode  - a bitwise or of values describing what is written to the file
 *             OBJ_NONE     -  render with only vertices
 *             OBJ_FLAT     -  render with facet normals
 *             OBJ_SMOOTH   -  render with vertex normals
 *             OBJ_TEXTURE  -  render with texture coords
 *             OBJ_COLOR    -  render with colors (color material)
 *             OBJ_MATERIAL -  render with materials
 *             OBJ_COLOR and OBJ_MATERIAL should not both be specified.  
 *             OBJ_FLAT and OBJ_SMOOTH should not both be specified.  
 */
void
OBJWriteOBJ(OBJmodel* model, char* filename, uint32_t mode)
{
    uint32_t i;
    FILE* file;
    OBJgroup* group;
    
    assert(model);
    
    /* do a bit of warning */
    if (mode & OBJ_FLAT && !model->facetnorms) {
        printf("OBJWriteOBJ() warning: flat normal output requested "
            "with no facet normals defined.\n");
        mode &= ~OBJ_FLAT;
    }
    if (mode & OBJ_SMOOTH && !model->normals) {
        printf("OBJWriteOBJ() warning: smooth normal output requested "
            "with no normals defined.\n");
        mode &= ~OBJ_SMOOTH;
    }
    if (mode & OBJ_TEXTURE && !model->texcoords) {
        printf("OBJWriteOBJ() warning: texture coordinate output requested "
            "with no texture coordinates defined.\n");
        mode &= ~OBJ_TEXTURE;
    }
    if (mode & OBJ_FLAT && mode & OBJ_SMOOTH) {
        printf("OBJWriteOBJ() warning: flat normal output requested "
            "and smooth normal output requested (using smooth).\n");
        mode &= ~OBJ_FLAT;
    }
    if (mode & OBJ_COLOR && !model->materials) {
        printf("OBJWriteOBJ() warning: color output requested "
            "with no colors (materials) defined.\n");
        mode &= ~OBJ_COLOR;
    }
    if (mode & OBJ_MATERIAL && !model->materials) {
        printf("OBJWriteOBJ() warning: material output requested "
            "with no materials defined.\n");
        mode &= ~OBJ_MATERIAL;
    }
    if (mode & OBJ_COLOR && mode & OBJ_MATERIAL) {
        printf("OBJWriteOBJ() warning: color and material output requested "
            "outputting only materials.\n");
        mode &= ~OBJ_COLOR;
    }
    
    
    /* open the file */
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "OBJWriteOBJ() failed: can't open file \"%s\" to write.\n",
            filename);
        exit(1);
    }
    
    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront OBJ generated by OBJ library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  OBJ library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n");
    
    if (mode & OBJ_MATERIAL && model->mtllibname) {
        fprintf(file, "\nmtllib %s\n\n", model->mtllibname);
        OBJWriteMTL(model, filename, model->mtllibname);
    }
    
    /* spit out the vertices */
    fprintf(file, "\n");
    fprintf(file, "# %d vertices\n", model->numvertices);
    for (i = 1; i <= model->numvertices; i++) {
        fprintf(file, "v %f %f %f\n", 
            model->vertices[i][0],
            model->vertices[i][1],
            model->vertices[i][2]);
    }
    
    /* spit out the smooth/flat normals */
    if (mode & OBJ_SMOOTH) {
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numnormals);
        for (i = 1; i <= model->numnormals; i++) {
            fprintf(file, "vn %f %f %f\n", 
                model->normals[i][0],
                model->normals[i][1],
                model->normals[i][2]);
        }
    } else if (mode & OBJ_FLAT) {
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numfacetnorms);
        for (i = 1; i <= model->numfacetnorms; i++) {
            fprintf(file, "vn %f %f %f\n", 
                model->facetnorms[i][0],
                model->facetnorms[i][1],
                model->facetnorms[i][2]);
        }
    }
    
    /* spit out the texture coordinates */
    if (mode & OBJ_TEXTURE) {
        fprintf(file, "\n");
        fprintf(file, "# %d texcoords\n", model->numtexcoords);
        for (i = 1; i <= model->numtexcoords; i++) {
            fprintf(file, "vt %f %f\n", 
                model->texcoords[i][0],
                model->texcoords[i][1]);
        }
    }
    
    fprintf(file, "\n");
    fprintf(file, "# %d groups\n", model->numgroups);
    fprintf(file, "# %d faces (triangles)\n", model->numtriangles);
    fprintf(file, "\n");
    
    group = model->groups;
    while(group) {
        fprintf(file, "g %s\n", group->name);
        if (mode & OBJ_MATERIAL)
            fprintf(file, "usemtl %s\n", model->materials[group->material].name);
        for (i = 0; i < group->numtriangles; i++) {
            if (mode & OBJ_SMOOTH && mode & OBJ_TEXTURE) {
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                    T(group->triangles[i]).vindices[0], 
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).nindices[0], 
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).tindices[2],
                    T(group->triangles[i]).nindices[2]);
            } else if (mode & OBJ_FLAT && mode & OBJ_TEXTURE) {
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            } else if (mode & OBJ_TEXTURE) {
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).tindices[2]);
            } else if (mode & OBJ_SMOOTH) {
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).nindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).vindices[2], 
                    T(group->triangles[i]).nindices[2]);
            } else if (mode & OBJ_FLAT) {
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0], 
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            } else {
                fprintf(file, "f %d %d %d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).vindices[2]);
            }
        }
        fprintf(file, "\n");
        group = group->next;
    }
    
    fclose(file);
}

/* OBJWeld: eliminate (weld) vectors that are within an epsilon of
 * each other.
 *
 * model   - initialized OBJmodel structure
 * epsilon     - maximum difference between vertices
 *               ( 0.00001 is a good start for a unitized model)
 *
 */
void
OBJWeld(OBJmodel* model, float epsilon)
{
    Vec3f_t *vectors;
    Vec3f_t *copies;
    uint32_t numvectors;
    uint32_t i;
    
    /* vertices */
    numvectors = model->numvertices;
    vectors  = model->vertices;
    copies = OBJWeldVectors(vectors, &numvectors, epsilon);
    
#if 0
    printf("OBJWeld(): %d redundant vertices.\n", 
        model->numvertices - numvectors - 1);
#endif
    
    for (i = 0; i < model->numtriangles; i++) {
        T(i).vindices[0] = (uint32_t)vectors[T(i).vindices[0]][0];
        T(i).vindices[1] = (uint32_t)vectors[T(i).vindices[1]][0];
        T(i).vindices[2] = (uint32_t)vectors[T(i).vindices[2]][0];
    }
    
    /* free space for old vertices */
    FREE(vectors);
    
    /* allocate space for the new vertices */
    model->numvertices = numvectors;
    model->vertices = NEWVEC(Vec3f_t, model->numvertices + 1);
    
    /* copy the optimized vertices into the actual vertex list */
    for (i = 1; i <= model->numvertices; i++) {
	CopyV3f (copies[i], model->vertices[i]);
    }
    
    FREE(copies);
}
