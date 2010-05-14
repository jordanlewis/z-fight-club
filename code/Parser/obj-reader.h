/*    
      obj-reader.h
      Nate Robins, 1997, 2000
      nate@pobox.com, http://www.pobox.com/~nate
 
      Wavefront OBJ model file format reader/writer/manipulator.

      Includes routines for generating smooth normals with
      preservation of edges, welding redundant vertices & texture
      coordinate generation (spheremap and planar projections) + more.

 */

#ifndef _OBJ_READER_H_
#define _OBJ_READER_H_

#include "Utilities/defs.h"
#include "Utilities/vector.h"
#include "Graphics/color.h"

#define OBJ_NONE     (0)            /* render with only vertices */
#define OBJ_FLAT     (1 << 0)       /* render with facet normals */
#define OBJ_SMOOTH   (1 << 1)       /* render with vertex normals */
#define OBJ_TEXTURE  (1 << 2)       /* render with texture coords */
#define OBJ_COLOR    (1 << 3)       /* render with colors */
#define OBJ_MATERIAL (1 << 4)       /* render with materials */


/* OBJmaterial: Structure that defines a material in a model. 
 */
typedef struct _OBJmaterial
{
  char		*name;		/* name of material */
  Color4_t	diffuse;	/* diffuse component */
  Color4_t	ambient;	/* ambient component */
  Color4_t	specular;	/* specular component */
  Color4_t	emmissive;	/* emmissive component */
  float		shininess;	/* specular exponent */
} OBJmaterial;

/* OBJtriangle: Structure that defines a triangle in a model.
 */
typedef struct _OBJtriangle {
  uint32_t	vindices[3];	/* array of triangle vertex indices */
  uint32_t	nindices[3];	/* array of triangle normal indices */
  uint32_t	tindices[3];	/* array of triangle texcoord indices*/
  uint32_t	findex;		/* index of triangle facet normal */
} OBJtriangle;

/* OBJgroup: Structure that defines a group in a model.
 */
typedef struct _OBJgroup {
  char*		name;		/* name of this group */
  uint32_t	numtriangles;	/* number of triangles in this group */
  uint32_t	*triangles;	/* array of triangle indices */
  uint32_t	material;	/* index to material for group */
  struct _OBJgroup *next;	/* pointer to next group in model */
} OBJgroup;

/* OBJmodel: Structure that defines a model.  Note that the vertex/normal/texcoord
 * arrays have 1-based indices, so the 0th element is unused.
 */
typedef struct _OBJmodel {
  char*		pathname;	/* path to this model */
  char*		mtllibname;	/* name of the material library */

  uint32_t	numvertices;	/* number of vertices in model */
  Vec3f_t	*vertices;	/* array of vertices  */

  uint32_t	numnormals;	/* number of normals in model */
  Vec3f_t	*normals;	/* array of normals */

  uint32_t	numtexcoords;	/* number of texcoords in model */
  Vec2f_t	*texcoords;	/* array of texture coordinates */

  uint32_t	numfacetnorms;	/* number of facetnorms in model */
  Vec3f_t	*facetnorms;	/* array of facetnorms */

  uint32_t	numtriangles;	/* number of triangles in model */
  OBJtriangle*	triangles;	/* array of triangles */

  uint32_t	nummaterials;	/* number of materials in model */
  OBJmaterial*	materials;	/* array of materials */

  uint32_t	numgroups;	/* number of groups in model */
  OBJgroup*	groups;		/* linked list of groups */

  Vec3f_t	position;	/* position of the model */

} OBJmodel;


/* OBJUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.  Returns the
 * scalefactor used.
 *
 * model - properly initialized OBJmodel structure 
 */
float
OBJUnitize(OBJmodel* model);

/* OBJDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model      - initialized OBJmodel structure
 * dimensions - array of 3 floats
 */
void
OBJDimensions(OBJmodel* model, Vec3f_t dimensions);

/* OBJScale: Scales a model by a given amount.
 * 
 * model - properly initialized OBJmodel structure
 * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
 */
void
OBJScale(OBJmodel* model, float scale);

/* OBJReverseWinding: Reverse the polygon winding for all polygons in
 * this model.  Default winding is counter-clockwise.  Also changes
 * the direction of the normals.
 * 
 * model - properly initialized OBJmodel structure 
 */
void
OBJReverseWinding(OBJmodel* model);

/* OBJFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized OBJmodel structure
 */
void
OBJFacetNormals(OBJmodel* model);

/* OBJVertexNormals: Generates smooth vertex normals for a model.
 * First builds a list of all the triangles each vertex is in.  Then
 * loops through each vertex in the the list averaging all the facet
 * normals of the triangles each vertex is in.  Finally, sets the
 * normal index in the triangle for the vertex to the generated smooth
 * normal.  If the dot product of a facet normal and the facet normal
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
OBJVertexNormals(OBJmodel* model, float angle);

/* OBJLinearTexture: Generates texture coordinates according to a
 * linear projection of the texture map.  It generates these by
 * linearly mapping the vertices onto a square.
 *
 * model - pointer to initialized OBJmodel structure
 */
void
OBJLinearTexture(OBJmodel* model);

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
OBJSpheremapTexture(OBJmodel* model);

/* OBJDelete: Deletes a OBJmodel structure.
 *
 * model - initialized OBJmodel structure
 */
void
OBJDelete(OBJmodel* model);

/* OBJReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * OBJDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.  
 */
OBJmodel* 
OBJReadOBJ(const char* filename);

/* OBJWriteOBJ: Writes a model description in Wavefront .OBJ format to
 * a file.
 *
 * model    - initialized OBJmodel structure
 * filename - name of the file to write the Wavefront .OBJ format data to
 * mode     - a bitwise or of values describing what is written to the file
 *            OBJ_NONE    -  write only vertices
 *            OBJ_FLAT    -  write facet normals
 *            OBJ_SMOOTH  -  write vertex normals
 *            OBJ_TEXTURE -  write texture coords
 *            OBJ_FLAT and OBJ_SMOOTH should not both be specified.
 */
void
OBJWriteOBJ(OBJmodel* model, char* filename, uint32_t mode);

/* OBJWeld: eliminate (weld) vectors that are within an epsilon of
 * each other.
 *
 * model      - initialized OBJmodel structure
 * epsilon    - maximum difference between vertices
 *              ( 0.00001 is a good start for a unitized model)
 *
 */
void
OBJWeld(OBJmodel* model, float epsilon);

#endif /*! _OBJ_READER_H_ */
