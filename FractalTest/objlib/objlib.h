#ifndef __OBJLIB_H__
#define __OBJLIB_H__

#include <stdio.h>

/*#define GLM_NONE     (0)            // render with only vertices 
#define GLM_FLAT     (1 << 0)       // render with facet normals 
#define GLM_SMOOTH   (1 << 1)       // render with vertex normals 
#define GLM_TEXTURE  (1 << 2)       // render with texture coords 
#define GLM_COLOR    (1 << 3)       // render with colors 
#define GLM_MATERIAL (1 << 4)       // render with materials 
#define GLM_LINE     (1 << 5)		// render with glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) 
#define T(x) (model->triangles[(x)])*/

typedef struct _OLNode 
{
    unsigned int index;
    bool averaged;
    struct _OLNode* next;
} OLNode;

typedef struct _OLMaterial
{
  char* name;                   /* name of material */
  float diffuse[4];           /* diffuse component */
  float ambient[4];           /* ambient component */
  float specular[4];          /* specular component */
  float emmissive[4];         /* emmissive component */
  float shininess;            /* specular exponent */
} OLMaterial;

typedef struct _OLTriangle 
{
  unsigned int vindices[3];           /* array of triangle vertex indices */
  unsigned int nindices[3];           /* array of triangle normal indices */
  unsigned int tindices[3];           /* array of triangle texcoord indices*/
  unsigned int findex;                /* index of triangle facet normal */
} OLTriangle;

typedef struct _OLGroup 
{
  char*             name;           /* name of this group */
  unsigned int            numtriangles;   /* number of triangles in this group */
  unsigned int*           triangles;      /* array of triangle indices */
  unsigned int            material;       /* index to material for group */
  struct _OLGroup* next;           /* pointer to next group in model */
} OLGroup;

class OLModel
{

private:

	void FirstPass(FILE*);
	void SecondPass(FILE*);
	OLGroup* FindGroup(char*);
	OLGroup* AddGroup(char*);
	unsigned int FindMaterial(char*);
	bool ReadMTL(char*);

public:

  char*    pathname;            /* path to this model */
  char*    mtllibname;          /* name of the material library */

  unsigned int   numvertices;         /* number of vertices in model */
  float* vertices;            /* array of vertices  */

  unsigned int   numnormals;          /* number of normals in model */
  float* normals;             /* array of normals */

  unsigned int   numtexcoords;        /* number of texcoords in model */
  float* texcoords;           /* array of texture coordinates */

  unsigned int   numfacetnorms;       /* number of facetnorms in model */
  float* facetnorms;          /* array of facetnorms */

  unsigned int       numtriangles;    /* number of triangles in model */
  OLTriangle* triangles;       /* array of triangles */

  unsigned int       nummaterials;    /* number of materials in model */
  OLMaterial* materials;       /* array of materials */

  unsigned int       numgroups;       /* number of groups in model */
  OLGroup*    groups;          /* linked list of groups */

  float position[3];          /* position of the model */

  
  OLModel();
  ~OLModel();

  bool Read(char*);
  void ReverseWinding(void);
  void BuildFacetNormals(void);
  void BuildVertexNormals(float);
  void Weld(float);
  void Draw(void);

};

/* glmUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.  Returns the
 * scalefactor used.
 *
 * model - properly initialized OLModel structure 
 */
//GLfloat glmUnitize(OLModel* model);

/* glmDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model      - initialized OLModel structure
 * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
 */

//GLvoid glmDimensions(OLModel* model, GLfloat* dimensions);

/* glmScale: Scales a model by a given amount.
 * 
 * model - properly initialized OLModel structure
 * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
 */

//GLvoid glmScale(OLModel* model, GLfloat scale);

//GLvoid glmLinearTexture(OLModel* model);
//GLvoid glmSpheremapTexture(OLModel* model);

//GLvoid glmDelete(OLModel* model);
//GLvoid glmWriteOBJ(OLModel* model, char* filename, GLuint mode);

//GLuint glmList(OLModel* model, GLuint mode);

#endif
