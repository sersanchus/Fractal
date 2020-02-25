#if defined(_MSC_VER)
	#if (_MSC_VER >= 1400)
		#define _CRT_SECURE_NO_DEPRECATE
		#include <windows.h>
	#else
		#include <windows.h>
		#define Sleep _sleep
	#endif	
	#include <io.h>
	#include "../FractalLib/FLFractalQuad.h"
	#include "../FractalLib/FLFractalConst.h"	
#elif defined(__CYGWIN__) || defined(__MINGW32__)
	#include <windows.h>	
	#define Sleep _sleep	
	#include <io.h>
	#include "FLFractalQuad.h"
	#include "FLFractalConst.h"	
#else
	#include <sys/time.h>
	#include "FLFractalQuad.h"
	#include "FLFractalConst.h"
	#define Sleep(x) usleep((x)*1000)
#endif

#include <fstream>
#include <iostream>
#include "Frustum.h"
#include "Point.h"
#include "Triangle.h"

using namespace std;

Frustum* fr;
Frustum* frfreze;
FLFractal2* fractal;
/*FLFractal2* fractalnr;
FLFractal2* fractalng;
FLFractal2* fractalnb;*/
char ficherofractal[256]="mdt512.fra2";//"clouds256.fra2";
char ficheroraw[256]="test.raw";//"clouds256.raw";
char* rawelev;
int width=500;
int height=500;
float precisionbusqueda=0.002f; // medio nivel de gris

unsigned int numpoolpoints=6000;
Point* poolpoints; // array de puntos a utilizar Point[numpoolpoints]

Point* freepoint; // puntos libres
unsigned int numfreepoints;

Point* points; // puntos en lista de utilizados y refinandonse
unsigned int numpoints;
Point* refinepoint; // punto a refinar, va recorriendo points de inicio a fin y vuelta a empezar

unsigned int numpooltriangles=20000;
Triangle* pooltriangles;
Triangle* freetriangle;
unsigned int numfreetriangles;
Triangle* drawtriangle; // Triangles a pintar
unsigned int numdrawtriangles; // número de triángulos en la lista de pintados
Triangle* triangletree1;
Triangle* triangletree2;
Triangle* bigger;
Triangle* smaller;
Triangle* refinetriangle;

unsigned int maxtriangles=1000; // triangulos a pintar en pantalla
bool debugwireframe=true;
bool debugnormals=false;

//#define FRACTALSHADER // si se quiere utilizar textura con un shader

#ifdef FRACTALSHADER
#include "glsl/glsl.h"
#define FRAG_FILE "fractal.frag"
#define VERT_FILE "fractal.vert"
#define FRACTAL_FILE "tile1-256.fra2"
glShaderManager SM;
glShader* shader=NULL;
GLuint texture_dompos;
GLuint texture_ori;
GLuint texture_so;
#endif

#include <GL/glut.h>

void testtri(Triangle* taux)
{
	if (taux->t0 && taux->t0->t1!=taux)
		printf("ERROR");
	if (taux->t1 && taux->t1->t0!=taux)
		printf("ERROR");
	if (taux->r0 && taux->r0->r1!=taux)
		printf("ERROR");
	if (taux->r1 && taux->r1->r0!=taux)
		printf("ERROR");
	if (taux->father && !(taux->father->rightc==taux || taux->father->leftc==taux))
		printf("ERROR");
	if (taux->rightc || taux->leftc)
		printf("ERROR");
	if (taux->basen && (taux->basen->rightc || taux->basen->leftc))
		printf("ERROR");
	if (taux->rightn && (taux->rightn->rightc || taux->rightn->leftc))
		printf("ERROR");
	if (taux->leftn && (taux->leftn->rightc || taux->leftn->leftc))
		printf("ERROR");
	if (taux->rightc && (taux->rightc->rightc || taux->rightc->leftc))
		printf("ERROR");
	if (taux->leftc && (taux->leftc->rightc || taux->leftc->leftc))
		printf("ERROR");
}

void test(void)
{
	unsigned int iaux;
	Point* paux;
	Triangle* taux;

	iaux=0;
	paux=points;
	if (paux->p0)
		printf("ERROR");
	while(paux)
	{
		iaux++;
		paux=paux->p1;
	}
	if (numpoints!=iaux)
		printf("ERROR");

	testtri(smaller);
	testtri(bigger);

	iaux=0;
	taux=drawtriangle;
	if (taux->t0)
		printf("ERROR");
	while(taux)
	{
		testtri(taux);
		iaux++;
		taux=taux->t1;
	}
	//if (numdrawtriangles!=iaux)
	//	printf("ERROR");

	iaux=0;
	taux=smaller;
	if (smaller->r0)
		printf("ERROR");
	while(taux)
	{
		testtri(taux);
		iaux++;
		if (taux->r1 && taux->r1->metric<taux->metric)
			printf("ERROR");
		taux=taux->r1;
	}
	//if (iaux!=numdrawtriangles)
	//	printf("ERROR");

	iaux=0;
	taux=bigger;
	if (bigger->r1)
		printf("ERROR");
	while(taux)
	{
		testtri(taux);
		iaux++;
		if (taux->r0 && taux->r0->metric>taux->metric)
			printf("ERROR");
		taux=taux->r0;
	}
	//if (iaux!=numdrawtriangles)
	//	printf("ERROR");
}
void orderpriorities(Triangle* taux1,Triangle* t) // triangulo y pivote
{
	/*taux1->metric=taux1->realmetric;
	if (taux1->cull==CULL_OUT)
		taux1->metric*=0.001f;*/

	if (taux1->r0 || taux1->r1)
	{
		if (taux1==bigger) 
		{	
			bigger=taux1->r0;
			if (t==taux1)
				t=bigger;
		}
		if (taux1==smaller)
		{
			smaller=taux1->r1;
			if (t==taux1)
				t=smaller;
		}
		if (taux1->r0) taux1->r0->r1=taux1->r1;
		if (taux1->r1) taux1->r1->r0=taux1->r0;
		taux1->r0=taux1->r1=0x0;
	}

	Triangle* qsearch=t;
	if (qsearch->metric<taux1->metric)
	{
		qsearch=qsearch->r1;
		while (qsearch)
		{
			if (qsearch->metric<taux1->metric)
				qsearch=qsearch->r1;
			else
				break;
		}
		if (!qsearch)
		{
			bigger->r1=taux1;
			taux1->r0=bigger;
			bigger=taux1;
		}
		else
		{
			taux1->r1=qsearch;
			taux1->r0=qsearch->r0;
			if (qsearch->r0) qsearch->r0->r1=taux1;
			qsearch->r0=taux1;
		}
	}
	else
	{
		qsearch=qsearch->r0;
		while (qsearch)
		{
			if (qsearch->metric>taux1->metric)
				qsearch=qsearch->r0;
			else
				break;
		}
		if (!qsearch)
		{
			smaller->r0=taux1;
			taux1->r1=smaller;
			smaller=taux1;
		}
		else
		{
			taux1->r0=qsearch;
			taux1->r1=qsearch->r1;
			if (qsearch->r1) qsearch->r1->r0=taux1;
			qsearch->r1=taux1;
		}
	}
}


void print(char* string)
{
	char* aux=string;
	while(*aux!=0)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
		aux++;
	}
}


void draw(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&frfreze->vmp.col[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_LIGHTING);
	Triangle* auxt=drawtriangle;
	glColor3f(1.0f,1.0f,1.0f);
	glPolygonMode(GL_FRONT,GL_FILL);

	#ifdef FRACTALSHADER
	glEnable(GL_TEXTURE_2D);
	shader->begin();
	#endif

	glBegin(GL_TRIANGLES);
	while(auxt)
	{
		if (auxt->cull==CULL_OUT)
		{
			auxt=auxt->t1;
			continue;
		}

		//glColor3fv(&auxt->p1->c.x);
		glNormal3fv(&auxt->p1->n.x);
		#ifdef FRACTALSHADER
		glTexCoord2fv(&auxt->p1->t.x);
		#endif
		glVertex3fv(&auxt->p1->v.x);
		//glColor3fv(&auxt->p2->c.x);
		glNormal3fv(&auxt->p2->n.x);
		#ifdef FRACTALSHADER
		glTexCoord2fv(&auxt->p2->t.x);
		#endif
		glVertex3fv(&auxt->p2->v.x);
		//glColor3fv(&auxt->p3->c.x);
		glNormal3fv(&auxt->p3->n.x);
		#ifdef FRACTALSHADER
		glTexCoord2fv(&auxt->p3->t.x);
		#endif
		glVertex3fv(&auxt->p3->v.x);
		auxt=auxt->t1;
	}
	glEnd();

	#ifdef FRACTALSHADER
	glDisable(GL_TEXTURE_2D);
	shader->end();
	#endif

	glDisable(GL_LIGHTING);

	if (debugnormals)
	{
		auxt=drawtriangle;
		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINES);
		while(auxt)
		{
			if (auxt->cull==CULL_OUT)
			{
				auxt=auxt->t1;
				continue;
			}

			glVertex3fv(&auxt->p1->v.x);
			glVertex3f(auxt->p1->v.x+auxt->p1->n.x*0.04f,auxt->p1->v.y+auxt->p1->n.y*0.04f,auxt->p1->v.z+auxt->p1->n.z*0.04f);
			glVertex3fv(&auxt->p2->v.x);
			glVertex3f(auxt->p2->v.x+auxt->p2->n.x*0.04f,auxt->p2->v.y+auxt->p2->n.y*0.04f,auxt->p2->v.z+auxt->p2->n.z*0.04f);
			glVertex3fv(&auxt->p3->v.x);
			glVertex3f(auxt->p3->v.x+auxt->p3->n.x*0.04f,auxt->p3->v.y+auxt->p3->n.y*0.04f,auxt->p3->v.z+auxt->p3->n.z*0.04f);
			
			auxt=auxt->t1;
		}
		glEnd();
	}
	
	if (debugwireframe)
	{
		auxt=drawtriangle;
		glColor3f(0.2f,0.2f,0.2f);
		glPolygonMode(GL_FRONT,GL_LINE);
		glBegin(GL_TRIANGLES);
		while(auxt)
		{
			//glColor3f(1.0f,0.2f,0.2f);
			glColor3f(0.0f,0.7f,0.0f);
			if (auxt->cull==CULL_OUT)
			{
				glColor3f(0.4f,0.4f,0.4f);
				//auxt=auxt->t1;
				//continue;
			}

			glVertex3fv(&auxt->p1->v.x);
			glVertex3fv(&auxt->p2->v.x);
			glVertex3fv(&auxt->p3->v.x);
			auxt=auxt->t1;
		}
		glEnd();
	}

	glColor3f(0.0f,0.0f,1.0f);

	// frustum
	glBegin(GL_LINE_STRIP);
	glVertex3fv(fr->frust_corners[0]);
	glVertex3fv(fr->frust_corners[1]);
	glVertex3fv(fr->frust_corners[3]);
	glVertex3fv(fr->frust_corners[2]);
	glVertex3fv(fr->frust_corners[0]);
	glVertex3fv(fr->frust_corners[4]);
	glVertex3fv(fr->frust_corners[5]);
	glVertex3fv(fr->frust_corners[7]);
	glVertex3fv(fr->frust_corners[6]);
	glVertex3fv(fr->frust_corners[4]);
	glEnd();	
	glBegin(GL_LINES);
	glVertex3fv(fr->frust_corners[3]);
	glVertex3fv(fr->frust_corners[7]);
	glVertex3fv(fr->frust_corners[2]);
	glVertex3fv(fr->frust_corners[6]);
	glVertex3fv(fr->frust_corners[1]);
	glVertex3fv(fr->frust_corners[5]);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f,width,height,0.0f);
	glMatrixMode(GL_MODELVIEW);
	char buffer[256]="\0";

	glRasterPos2i(5,15);	
	sprintf(buffer,"tris= %d | %d",numpooltriangles-numfreetriangles,numpooltriangles);
	print(buffer);
	
	glRasterPos2i(5,30);
	sprintf(buffer,"points= %d | %d",numpoolpoints-numfreepoints,numpoolpoints);
	print(buffer);

	glRasterPos2i(5,45);
	sprintf(buffer,"ndrawt= %d | %d",numdrawtriangles,maxtriangles);
	print(buffer);

	glFlush();
	glutSwapBuffers();
}

void updateculltriangle(Triangle* t)
{
	#define REDUCCIONPORCULL 0.1f

	unsigned char cullantes=t->cull;
	
	fr->UpdateCull(t);

	if (t->rightc || t->leftc)
		return;

	if (t->cull!=cullantes)
	{
		if (t->cull==CULL_OUT)
		{
			t->metric=t->realmetric*REDUCCIONPORCULL;

			orderpriorities(t,smaller);
						
			numdrawtriangles--;
		}
		else if (cullantes==CULL_OUT)
		{	
			t->metric=t->realmetric;

			orderpriorities(t,bigger);

			numdrawtriangles++;
		}
	}
}

void updateculltrianglerecurs(Triangle* t)
{
	unsigned char auxbyte=t->cull;
	
	updateculltriangle(t);
	
	// si el arbol sigue siendo igual no hacer nada
	if (auxbyte==t->cull && (auxbyte==CULL_OUT || auxbyte==CULL_ALLIN))
		return;

	if (t->rightc) updateculltrianglerecurs(t->rightc);
	if (t->leftc) updateculltrianglerecurs(t->leftc);
}

void updatecull(void)
{
	updateculltrianglerecurs(triangletree1);
	updateculltrianglerecurs(triangletree2);
}

long ticks()
{
#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	return GetTickCount();
#else
	timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000L+tv.tv_usec/1000L;
#endif
}


Point* nuevopunto(float x,float y,float z)
{
	Point* ret=freepoint;
	freepoint->p1->p0=0x0;
	freepoint=freepoint->p1;
	numfreepoints--;

	ret->v.x=x;
	ret->v.y=y;
	ret->v.z=z;
	ret->p0=ret->p1=0x0;
	ret->quality=0.0f;

	ret->n=Vector3(0.0f,1.0f,0.0f);
	
/*	unsigned int pasos;
	ret->n.x=(fractalnr->Decode(FLVector2(x,1.0f+z),0.01f,1.0f,pasos)-0.5f)*2.0f;
	ret->n.y=(fractalnb->Decode(FLVector2(x,1.0f+z),0.01f,1.0f,pasos)-0.5f)*2.0f;
	ret->n.z=(fractalng->Decode(FLVector2(x,1.0f+z),0.01f,1.0f,pasos)-0.5f)*2.0f;
*/
	//ret->n=ret->n.Normalize();
	/*aux->c.x=fractal->Decode(FLVector2(aux->v.x,1.0f+aux->v.z),precisionbusqueda,255.0f,pasos)/255.0f;
	aux->c.y=fractal->Decode(FLVector2(aux->v.x,1.0f+aux->v.z),precisionbusqueda,255.0f,pasos)/255.0f;
	aux->c.z=fractal->Decode(FLVector2(aux->v.x,1.0f+aux->v.z),precisionbusqueda,255.0f,pasos)/255.0f;*/
	
	return ret;
}

void borrapunto(Point* p)
{
	freepoint->p0=p;
	p->p1=freepoint;
	freepoint=p;

	numfreepoints++;
}

Triangle* nuevotriangulo(Point* p1,Point* p2,Point* p3)
{
	Triangle* ret=freetriangle;
	freetriangle->t1->t0=0x0;
	freetriangle=freetriangle->t1;
	numfreetriangles--;

	ret->p1=p1;
	ret->p2=p2;
	ret->p3=p3;
	ret->rightc=ret->leftc=ret->rightn=ret->leftn=ret->basen=ret->father=ret->r0=ret->r1=ret->t0=ret->t1=0x0;
	ret->level=0;
	ret->cull=CULL_OUT;
	
	ret->UpdateRBound();

	float auxx=fr->position.x-ret->center.x;
	float auxy=fr->position.y-ret->center.y;
	float auxz=fr->position.z-ret->center.z;
	ret->metric=ret->realmetric=fr->q_pix*ret->rbound/(auxx*auxx+auxy*auxy+auxz*auxz);

	return ret;
}
void borratriangulo(Triangle* t)
{
	freetriangle->t0=t;
	t->t1=freetriangle;
	freetriangle=t;

	numfreetriangles++;
}
void quitartriangulodelistas(Triangle* t)
{
	if (t==refinetriangle)
	{
		refinetriangle=t->t1;
		if (!refinetriangle)
			refinetriangle=drawtriangle;
	}
	if (t->t1) t->t1->t0=t->t0;
	if (t->t0) t->t0->t1=t->t1;
	if (t==smaller) smaller=t->r1;
	if (t==bigger) bigger=t->r0;
	if (t->r1) t->r1->r0=t->r0;			
	if (t->r0) t->r0->r1=t->r1;

	t->t0=t->t1=t->r0=t->r1=0x0;
}
void quitarpuntodelistas(Point* p)
{
	if (p==points)
	{
		points=points->p1;
		points->p0=NULL;
	}
	else
	{
		p->p0->p1=p->p1;
		if (p->p1) 
			p->p1->p0=p->p0;
	}
	if (p==refinepoint)
	{
		refinepoint=p->p1;
		if (!refinepoint)
			refinepoint=points;
	}

	p->p0=p->p1=0x0;

	numpoints--;
}

void split2(Triangle* t)
{
	// si el vecino base está spliteado ya coger su punto medio sino lo creo nuevo
	Point* pmedio;
	if (t->basen && t->basen->rightc)
	{
		pmedio=t->basen->rightc->p2;
	}
	else
	{
		/*Vector3 medio1=t->p1->v+(t->p3->v-t->p1->v)*0.4f;
		Vector3 medio2=t->p1->v+(t->p3->v-t->p1->v)*0.5f;
		Vector3 medio3=t->p1->v+(t->p3->v-t->p1->v)*0.6f;
		float aux1,aux2,aux3,dif1,dif2,dif3;		
		aux1=fractal->Decode(FLVector2(medio1.x,1.0f+medio1.z),10.0f+precisionbusqueda,medio1.y*5000.0f,pasos)/5000.0f;
		dif1=fabs(medio1.y-aux1);
		aux2=fractal->Decode(FLVector2(medio2.x,1.0f+medio2.z),10.0f+precisionbusqueda,medio2.y*5000.0f,pasos)/5000.0f;
		dif2=fabs(medio2.y-aux2);
		aux3=fractal->Decode(FLVector2(medio3.x,1.0f+medio3.z),10.0f+precisionbusqueda,medio3.y*5000.0f,pasos)/5000.0f;
		dif3=fabs(medio3.y-aux3);
		if (dif2>=dif1 && dif2>=dif3) 
		{
			medio2.y=aux2;
			pmedio=new Point(medio2);
		}
		else if (dif1>=dif2 && dif1>=dif3) 
		{
			medio1.y=aux1;
			pmedio=new Point(medio1);
		}
		else if (dif3>=dif1 && dif3>=dif2) 
		{
			medio3.y=aux3;
			pmedio=new Point(medio3);
		}*/

		Vector3 medio=(t->p1->v+t->p3->v)*0.5f;
		pmedio=nuevopunto(medio.x,medio.y,medio.z);
		pmedio->trianglefather=t;
		pmedio->t=(t->p1->t+t->p3->t)*0.5f;
		points->p0=pmedio;
		pmedio->p1=points;
		points=pmedio;
		numpoints++;
	}

	// creo hijos y los añado a las listas
	t->rightc=nuevotriangulo(t->p2,pmedio,t->p1);
	t->leftc=nuevotriangulo(t->p3,pmedio,t->p2);
	t->rightc->level=t->leftc->level=t->level+1;
	t->rightc->father=t->leftc->father=t;
	t->leftc->leftn=t->rightc;
	t->rightc->rightn=t->leftc;
	t->leftc->basen=t->leftn;
	if (t->leftn)
	{
		if (t->leftn->basen==t)
			t->leftn->basen=t->leftc;
		else if (t->leftn->leftn==t) 
			t->leftn->leftn=t->leftc;
		else 
			t->leftn->rightn=t->leftc;
	}
	t->rightc->basen=t->rightn;
	if (t->rightn)
	{
		if (t->rightn->basen==t)
			t->rightn->basen=t->rightc;
		else if (t->rightn->rightn==t) 
			t->rightn->rightn=t->rightc;
		else 
			t->rightn->leftn=t->rightc;
	}
	t->leftc->t1=t->rightc;
	t->rightc->t0=t->leftc;
	drawtriangle->t0=t->rightc;
	t->rightc->t1=drawtriangle;
	drawtriangle=t->leftc;

	// dos triángulos nuevos
	//numdrawtriangles+=2;

	updateculltriangle(t->rightc);
	updateculltriangle(t->leftc);
	
	// quito a t de las listas, el numdrawtriangles se decrementa
	quitartriangulodelistas(t);	

	if (t->cull!=CULL_OUT)
		numdrawtriangles--;
}

void split(Triangle* t)
{
	if (t->basen)
	{
		if (t->basen->basen!=t)	
		{
			split(t->basen);
		}
		split2(t);
		split2(t->basen);
		t->leftc->rightn=t->basen->rightc;
		t->rightc->leftn=t->basen->leftc;
		t->basen->leftc->rightn=t->rightc;
		t->basen->rightc->leftn=t->leftc;
	}
	else
	{
		split2(t);
	}	

	test();
}

void merge2(Triangle* t)
{
	// quitamos punto medio
	if (t->rightc->p2)
	{
		// quitamos punto de las listas y decrementamos numpoints
		quitarpuntodelistas(t->rightc->p2);

		borrapunto(t->rightc->p2);
		t->rightc->p2=NULL;
		t->leftc->p2=NULL;
		if (t->basen)
		{
			t->basen->rightc->p2=NULL;
			t->basen->leftc->p2=NULL;
		}
	}

	// añadir a él mismo
	drawtriangle->t0=t;
	t->t0=NULL;
	t->t1=drawtriangle;
	drawtriangle=t;

	fr->UpdateCull(t);
	if (t->cull!=CULL_OUT)
	{
		orderpriorities(t,t->rightc);
		numdrawtriangles++;
	}

	if (t->leftn)
	{
		if (t->leftn->basen==t->leftc)
			t->leftn->basen=t;
		else if (t->leftn->rightn==t->leftc)
			t->leftn->rightn=t;
		else
			t->leftn->leftn=t;
	}
	if (t->rightn)
	{
		if (t->rightn->basen==t->rightc)
			t->rightn->basen=t;
		else if (t->rightn->rightn==t->rightc)
			t->rightn->rightn=t;
		else
			t->rightn->leftn=t;
	}

	if (t->rightc->basen && t->rightc->basen->basen==t->rightc)
		t->rightc->basen->basen=t;
	if (t->leftc->basen && t->leftc->basen->basen==t->leftc)
		t->leftc->basen->basen=t;
	t->rightn=t->rightc->basen;
	t->leftn=t->leftc->basen;

	// quitar hijos 
	quitartriangulodelistas(t->rightc);
	if (t->rightc->cull!=CULL_OUT)
		numdrawtriangles--;
	quitartriangulodelistas(t->leftc);	
	if (t->leftc->cull!=CULL_OUT)
		numdrawtriangles--;

	borratriangulo(t->rightc);
	t->rightc=NULL;
	borratriangulo(t->leftc);
	t->leftc=NULL;

}

void merge(Triangle* t)
{
	// si no es triángulo base
	if (t && t->rightc && t->leftc)
	{
		merge(t->rightc);
		merge(t->leftc);
		if (t->basen)
		{
			merge(t->basen->rightc);
			merge(t->basen->leftc);
			merge2(t->basen);
			merge2(t);
		}
		else
		{
			merge2(t);
		}
	}
}

void refine(void)
{
	#define CALIDADINICIAL 0.01f
	#define PASOREFINAMIENTO 1.0f
	#define ALTURAMAXIMA 0.05f
	#define PUNTOSAREFINARPORFRAME 10

	//unsigned int pasos;

	for (unsigned int i=0;i<PUNTOSAREFINARPORFRAME;i++)
	{
		if (refinepoint->quality<1.0f)
		{
			unsigned int coordx=(unsigned int)(refinepoint->v.x*(fractal->imagetamx*4-1));
			unsigned int coordy=(unsigned int)((1.0f+refinepoint->v.z)*(fractal->imagetamy*4-1));
			refinepoint->v.y=(((unsigned char)rawelev[coordy*(fractal->imagetamx*4)+coordx])/255.0f)*ALTURAMAXIMA;
		
			//float precisionahora=precisionbusqueda*refinepoint->quality+CALIDADINICIAL*(1-refinepoint->quality);
			//refinepoint->v.y=fractal->Decode(FLVector2(refinepoint->v.x,1.0f+refinepoint->v.z),precisionahora,refinepoint->v.y,pasos)*ALTURAMAXIMA;
		
			/*refinepoint->n.x=((fractalnr->Decode(FLVector2(refinepoint->v.x,1.0f+refinepoint->v.z),10.0f+precisionbusqueda-10.0f*refinepoint->quality,(refinepoint->n.x*0.5f+0.5f)*255.0f,pasos)/255.0f)-0.5f)*2.0f;
			refinepoint->n.y=((fractalnb->Decode(FLVector2(refinepoint->v.x,1.0f+refinepoint->v.z),10.0f+precisionbusqueda-10.0f*refinepoint->quality,(refinepoint->n.y*0.5f+0.5f)*255.0f,pasos)/255.0f)-0.5f)*2.0f;
			refinepoint->n.z=((fractalng->Decode(FLVector2(refinepoint->v.x,1.0f+refinepoint->v.z),10.0f+precisionbusqueda-10.0f*refinepoint->quality,(refinepoint->n.z*0.5f+0.5f)*255.0f,pasos)/255.0f)-0.5f)*2.0f;*/

			refinepoint->quality+=PASOREFINAMIENTO;
		}

		if (refinepoint->trianglefather)
		{
			Vector3 normaltrian=(refinepoint->trianglefather->p3->v-refinepoint->trianglefather->p2->v)^(refinepoint->trianglefather->p1->v-refinepoint->trianglefather->p2->v);
			normaltrian=normaltrian.Normalize();
			refinepoint->n+=normaltrian;
			refinepoint->n=refinepoint->n.Normalize();
		}

		if (refinepoint->p1)
			refinepoint=refinepoint->p1;
		else
			refinepoint=points;

	}

	#define TRIANGULOSAREFINARPORFRAME 5
	
	for (unsigned int i=0;i<TRIANGULOSAREFINARPORFRAME;i++)
	{
		float faux=refinetriangle->realmetric;
		
		refinetriangle->UpdateRBound();
		float auxx=fr->position.x-refinetriangle->center.x;
		float auxy=fr->position.y-refinetriangle->center.y;
		float auxz=fr->position.z-refinetriangle->center.z;
		refinetriangle->metric=refinetriangle->realmetric=fr->q_pix*refinetriangle->rbound/(auxx*auxx+auxy*auxy+auxz*auxz);

		/*if (refinetriangle->cull==CULL_OUT)
			refinetriangle->metric*=REDUCCIONPORCULL;*/
		
		if (refinetriangle->realmetric!=faux)
		{
			orderpriorities(refinetriangle,smaller);
		}

		if (refinetriangle->t1)
			refinetriangle=refinetriangle->t1;
		else
			refinetriangle=drawtriangle;
	}
}

void optimize(void)
{	
	static Triangle* splitfallido;

	#define sigue (numdrawtriangles<maxtriangles && numfreetriangles>32 && numfreepoints>32)
	int nummaxoperations=20;
	Triangle* aux;
	while (nummaxoperations>0)
	{
		if (sigue)
		{
			aux=bigger;
			
			if (splitfallido==aux)
				break;
			
			split(aux);			
			if (!sigue && aux==smaller->father)
			{
				merge(aux);
				splitfallido=aux;
				break;
			}
			splitfallido=0x0;
		}
		else
		{
			merge(smaller->father);
			splitfallido=0x0;
		}
		nummaxoperations--;
	}
}


void update(void)
{
	static long ticksantes=ticks();
	long ticksahora=ticks();
	if ((ticksahora-ticksantes)>39)
	{
		refine();
		test();	
		
		/*optimize();
		test();*/

		draw();

		ticksantes=ticksahora;
	}
	else
	{
		Sleep(39-(ticksahora-ticksantes));
	}
}

void reshape(int w,int h)
{
	glViewport(0,0,w,h);
	width=w;
	height=h;
	fr->Reshape(w,h);
	fr->Update();
}


void key(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27: // esc
			exit(0);
		case 'z':
			fr->SetPosition(fr->GetPosition()-fr->GetDirection()*0.01f);
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case 'a':
			fr->SetPosition(fr->GetPosition()+fr->GetDirection()*0.01f);
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case 'Z':
			fr->SetPosition(fr->GetPosition()-fr->GetDirection()*0.1f);
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case 'A':
			fr->SetPosition(fr->GetPosition()+fr->GetDirection()*0.1f);
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case '+':
			for (unsigned int i=0;i<1;i++)
			{
				split(bigger);
				test();
			}
			glutPostRedisplay();
			break;
		case '-':
			for (unsigned int i=0;i<1;i++)
			{
				merge(smaller->father);
				test();
			}
			glutPostRedisplay();
			break;
		case 's':
			fr->SetPosition(fr->GetPosition()+Vector3(0.0f,0.05f,0.0f));
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case 'x':
			fr->SetPosition(fr->GetPosition()+Vector3(0.0f,-0.05f,0.0f));
			fr->Update();
			updatecull();
			glutPostRedisplay();
			break;
		case 'w':
			debugwireframe=!debugwireframe;
			break;
		case 'n':
			debugnormals=!debugnormals;
			break;
	}	
}

void move(Vector3 desp)
{
	Vector3 movex=Vector3(fr->right.x,0.0f,fr->right.z).Normalize();
	Vector3 movey=Vector3(0.0f,1.0f,0.0f);
	Vector3 movez=fr->up+fr->direction;
	movez.y=0.0f;
	movez=-movez.Normalize();
	desp=desp*(fr->position.y+1.0f);
	fr->position=fr->position+movex*desp.x+movey*desp.y+movez*desp.z;
	fr->Update();
	updatecull();
	glutPostRedisplay();
}
void special(int key, int x, int y)
{
	if (key==GLUT_KEY_LEFT)
	{
		move(Vector3(-0.01f,0.0f,0.0f));
	}
	else if (key==GLUT_KEY_RIGHT)
	{
		move(Vector3(0.01f,0.0f,0.0f));
	}
	else if (key==GLUT_KEY_UP)
	{
		move(Vector3(0.0f,0.0f,-0.01f));
	}
	else if (key==GLUT_KEY_DOWN)
	{
		move(Vector3(0.0f,0.0f,0.01f));
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button==GLUT_LEFT_BUTTON)
	{
		if (state==GLUT_DOWN)
		{
			fr->mx0=x;
			fr->my0=y;
		}
		else
		{
			fr->mx0=-1;
			fr->my0=-1;
		}
	}
}

void motion(int x, int y)
{
	if(fr->mx0!=-1 && fr->my0!=-1 && (fr->mx0!=x || fr->my0!=y))
	{
		fr->drag((float)fr->mx0,(float)fr->my0,(float)x,(float)y);
		fr->mx0=x;
		fr->my0=y;
		fr->Update();
		updatecull();
		glutPostRedisplay();
	}
}

void menu(int value)
{

}
int main(int argc,char **argv)
{
	/////////////////////
	// leo parámetros
	/////////////////////

	if (argc>1)
	{
		strncpy(ficherofractal,argv[1],256);
	}
	#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	if (_access(ficherofractal,0)==-1)
	#else
	if (access(ficherofractal,0)==-1)
	#endif
	{
		cout << ficherofractal << " doesn't exist" << endl;
		exit(1);
	}
	if (argc>2)
	{
		precisionbusqueda=(float)atof(argv[2]);
	}

	/////////////////////
	// cargo el fractal para el terreno
	/////////////////////

	FLFile file(ficherofractal,"r+b");
	char clave[8];
	clave[0]=file.Unpack(8);
	clave[1]=file.Unpack(8);
	clave[2]=file.Unpack(8);
	clave[3]=file.Unpack(8);
	clave[4]=file.Unpack(8);
	clave[5]=file.Unpack(8);
	clave[6]=file.Unpack(8);
	clave[7]='\0';
	unsigned char version;
	version=file.Unpack(8);
	unsigned char type;
	type=file.Unpack(8);
	unsigned short tamxentrada=file.Unpack(16);
	unsigned short tamyentrada=file.Unpack(16);
	bool color=(file.Unpack(1)==1);
	if (color)
	{
		printf("At the moment, give me only grayscale fractals\n");
		exit(1);
	}
	cout << "prec:\t" << precisionbusqueda << endl;
	cout << "file:\t" << ficherofractal << endl;
	cout << "key:\t" << clave << endl;
	cout << "rev:\t" << (int)version << endl;
	if (type==0)
		cout << "type:\t2d" << endl;
	else
		cout << "type:\t3d" << endl;
	cout << "width:\t" << tamxentrada << endl;
	cout << "height:\t" << tamyentrada << endl;
	cout << "type:\t";
	/*if (color)
		cout << conversion << " -> rgb" << endl;
	else*/
		cout << "grayscale" << endl;
	cout << endl;
	unsigned char tipofractal=(unsigned char)file.Unpack(8);
	if (tipofractal==FRACTAL_CONST)
		fractal=new FLFractalConst2();
	else if (tipofractal==FRACTAL_QUAD)
		fractal=new FLFractalQuad2();
	fractal->imagetamx=tamxentrada;
	fractal->imagetamy=tamyentrada;
	fractal->Load(file);

	rawelev=new char[tamxentrada*tamyentrada*16];
	std::ifstream fileraw(ficheroraw,std::ios_base::binary);
	fileraw.read(rawelev,tamxentrada*tamyentrada*16);
	fileraw.close();

	/*FLFile filenx("clouds256normalr.fra2","r+b");
	clave[0]=filenx.Unpack(8);clave[1]=filenx.Unpack(8);clave[2]=filenx.Unpack(8);clave[3]=filenx.Unpack(8);clave[4]=filenx.Unpack(8);clave[5]=filenx.Unpack(8);clave[6]=filenx.Unpack(8);clave[7]='\0';
	version=filenx.Unpack(8);
	type=filenx.Unpack(8);
	tamxentrada=filenx.Unpack(16);
	tamyentrada=filenx.Unpack(16);
	color=(filenx.Unpack(1)==1);
	tipofractal=(unsigned char)filenx.Unpack(8);
	if (tipofractal==FRACTAL_CONST)
		fractalnr=new FLFractalConst2();
	else if (tipofractal==FRACTAL_QUAD)
		fractalnr=new FLFractalQuad2();
	fractalnr->imagetamx=tamxentrada;
	fractalnr->imagetamy=tamyentrada;
	fractalnr->Load(filenx);

	FLFile fileny("clouds256normalg.fra2","r+b");
	clave[0]=fileny.Unpack(8);clave[1]=fileny.Unpack(8);clave[2]=fileny.Unpack(8);clave[3]=fileny.Unpack(8);clave[4]=fileny.Unpack(8);clave[5]=fileny.Unpack(8);clave[6]=fileny.Unpack(8);clave[7]='\0';
	version=fileny.Unpack(8);
	type=fileny.Unpack(8);
	tamxentrada=fileny.Unpack(16);
	tamyentrada=fileny.Unpack(16);
	color=(fileny.Unpack(1)==1);
	tipofractal=(unsigned char)fileny.Unpack(8);
	if (tipofractal==FRACTAL_CONST)
		fractalng=new FLFractalConst2();
	else if (tipofractal==FRACTAL_QUAD)
		fractalng=new FLFractalQuad2();
	fractalng->imagetamx=tamxentrada;
	fractalng->imagetamy=tamyentrada;
	fractalng->Load(fileny);

	FLFile filenz("clouds256normalb.fra2","r+b");
	clave[0]=filenz.Unpack(8);clave[1]=filenz.Unpack(8);clave[2]=filenz.Unpack(8);clave[3]=filenz.Unpack(8);clave[4]=filenz.Unpack(8);clave[5]=filenz.Unpack(8);clave[6]=filenz.Unpack(8);clave[7]='\0';
	version=filenz.Unpack(8);
	type=filenz.Unpack(8);
	tamxentrada=filenz.Unpack(16);
	tamyentrada=filenz.Unpack(16);
	color=(filenz.Unpack(1)==1);
	tipofractal=(unsigned char)filenz.Unpack(8);
	if (tipofractal==FRACTAL_CONST)
		fractalnb=new FLFractalConst2();
	else if (tipofractal==FRACTAL_QUAD)
		fractalnb=new FLFractalQuad2();
	fractalnb->imagetamx=tamxentrada;
	fractalnb->imagetamy=tamyentrada;
	fractalnb->Load(filenz);*/

	/////////////////////
	// preparo frustum
	/////////////////////

	Vector3 pos(0.5f,0.25f,-0.2f);
	//Vector3 pos(0.5f,1.25f,-0.5f);
	Vector3 to(0.5f,0.0f,-0.4f);
	Vector3 dir=(to-pos).Normalize();
	Vector3 up=(dir^Vector3(0.0f,1.0f,0.0f))^dir;
	//Vector3 up=Vector3(0.0f,0.0f,-1.0f);
	up=up.Normalize();
	fr=new Frustum(pos,dir,up,width,height);

	pos=Vector3(0.0f,1.0f,0.5f);
	//Vector3 pos(0.5f,1.25f,-0.5f);
	to=Vector3(0.5f,0.0f,-0.5f);
	dir=(to-pos).Normalize();
	up=(dir^Vector3(0.0f,1.0f,0.0f))^dir;
	//Vector3 up=Vector3(0.0f,0.0f,-1.0f);
	up=up.Normalize();
	frfreze=new Frustum(pos,dir,up,width,height);

	/////////////////////
	// preparo la lista de triángulos
	/////////////////////

	poolpoints=new Point[numpoolpoints];
	for (unsigned int jh=0;jh<numpoolpoints-1;jh++)
	{
		poolpoints[jh].p1=&poolpoints[jh+1];
		poolpoints[jh+1].p0=&poolpoints[jh];
	}
	freepoint=&poolpoints[0];
	numfreepoints=numpoolpoints;

	points=nuevopunto(0.0f,0.0f,-1.0f);
	points->trianglefather=NULL;
	points->t=Vector2(0.0f,0.0f);
	points->p0=nuevopunto(1.0f,0.0f,-1.0f);
	points->p0->trianglefather=NULL;
	points->p0->t=Vector2(1.0f,0.0f);
	points->p0->p1=points;
	points=points->p0;
	points->p0=nuevopunto(1.0f,0.0f,0.0f);
	points->p0->trianglefather=NULL;
	points->p0->t=Vector2(1.0f,1.0f);
	points->p0->p1=points;
	points=points->p0;
	points->p0=nuevopunto(0.0f,0.0f,0.0f);
	points->p0->trianglefather=NULL;
	points->p0->t=Vector2(0.0f,1.0f);
	points->p0->p1=points;
	points=points->p0;
	numpoints=4;

	pooltriangles=new Triangle[numpooltriangles];
	for (unsigned int jh=0;jh<numpooltriangles-1;jh++)
	{
		pooltriangles[jh].t1=&pooltriangles[jh+1];
		pooltriangles[jh+1].t0=&pooltriangles[jh];
	}
	freetriangle=&pooltriangles[0];
	numfreetriangles=numpooltriangles;

	triangletree1=nuevotriangulo(points,points->p1,points->p1->p1);
	triangletree2=nuevotriangulo(points->p1->p1,points->p1->p1->p1,points);		
	//numdrawtriangles=2;
	triangletree1->basen=triangletree2;
	triangletree2->basen=triangletree1;
	triangletree1->t1=triangletree2;
	triangletree2->t0=triangletree1;
	drawtriangle=triangletree1;

	smaller=triangletree1;
	bigger=triangletree1;
	orderpriorities(triangletree2,smaller);
	updateculltriangle(triangletree1);
	updateculltriangle(triangletree2);

	refinepoint=points;
	refinetriangle=drawtriangle;

	/////////////////////
	// main loop
	/////////////////////

	#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	glutInit(&argc,argv);
	#else
	int jg=0;
	glutInit(&jg,NULL);
	#endif
		
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width,height);
	glutCreateWindow("Fractal3d");

	/////////////////////
	// fractal shader
	/////////////////////

	#ifdef FRACTALSHADER

	checkGL2();
	shader = SM.loadfromFile(VERT_FILE, FRAG_FILE);
	
	FLFile filefractalshader(FRACTAL_FILE,"r+b");

	clave[0]=filefractalshader.Unpack(8);
	clave[1]=filefractalshader.Unpack(8);
	clave[2]=filefractalshader.Unpack(8);
	clave[3]=filefractalshader.Unpack(8);
	clave[4]=filefractalshader.Unpack(8);
	clave[5]=filefractalshader.Unpack(8);
	clave[6]=filefractalshader.Unpack(8);
	clave[7]='\0';

	if (strcmp("Fractal",clave)!=0)
	{
		cout << FRACTAL_FILE << " isn't a fractal" << endl;
		exit(1);
	}

	version=filefractalshader.Unpack(8);
	type=filefractalshader.Unpack(8);
	tamxentrada=filefractalshader.Unpack(16);
	tamyentrada=filefractalshader.Unpack(16);
	
	if (tamxentrada>256 || tamyentrada>256)
	{
		cout << "pasame un fractal de 256 de tamaño" << endl;
		exit(1);
	}	
	
	color=(filefractalshader.Unpack(1)==1);

	cout << "filefractalshader:\t" << FRACTAL_FILE << endl;
	cout << "key:\t" << clave << endl;
	cout << "rev:\t" << (int)version << endl;
	if (type==0)
		cout << "type:\t2d" << endl;
	else
		cout << "type:\t3d" << endl;
	cout << "width:\t" << tamxentrada << endl;
	cout << "height:\t" << tamyentrada << endl;
	cout << "color:\t";
	if (color)
		cout << "yiq" << " -> rgb" << endl;
	else
		cout << "grayscale" << endl;
	cout << endl;

	if (color)
	{
		cout << "pasame un fractal en escala de grises" << endl;
		exit(1);
	}
	else
	{
		unsigned char tipofractal=(unsigned char)filefractalshader.Unpack(8);
		if (tipofractal==FRACTAL_CONST)
		{
			unsigned short ransize=(unsigned short)filefractalshader.Unpack(16);

			/*if (ransize!=4)
			{
				cout << "pasame un fractal de ransize 4" << endl;
				exit(1);
			}*/

			unsigned short transx,transy;
			transx=tamxentrada/ransize;	
			transy=tamyentrada/ransize;	
			unsigned int numtransformations=transx*transy;

			unsigned char* domposptr=new unsigned char[numtransformations<<1];
			unsigned char* oriptr=new unsigned char[numtransformations];		
			unsigned char* soptr=new unsigned char[numtransformations<<1];

			unsigned int numtrans=0;
			for (unsigned short j=0;j<tamyentrada;j+=ransize)
			{
				for (unsigned short i=0;i<tamxentrada;i+=ransize)
				{
					unsigned short domposx=0;
					unsigned short domposy=0;
					unsigned char ori=0;
					float s=0.0f;
					float o=0.0f;

					unsigned char pis=(unsigned char)filefractalshader.Unpack(5);
					unsigned char pio=(unsigned char)filefractalshader.Unpack(7);
					if (pis!=16)
					{
						domposx=(unsigned short)filefractalshader.Unpack(16);
						domposy=(unsigned short)filefractalshader.Unpack(16);
						ori=(unsigned char)filefractalshader.Unpack(3);	
					}	
					else
					{
						int sdfa=2;
					}

					s=(float)pis/(float)(1<<5)*2.0f-1.0f;
					o=(float)pio/(float)((1<<7)-1)*((1.0f+fabs(s))*255.0f);
					if (s>0.0) 
						o-=s*255.0f;

					domposptr[numtrans<<1]=domposx;
					domposptr[(numtrans<<1)+1]=domposy;
					oriptr[numtrans]=ori<<5;
					soptr[numtrans<<1]=(unsigned char)(((s*0.5f)+0.5f)*255.0f+0.5f);
					
					float aux=(o*0.5f)+127.5f+0.5f;
					if (aux<0.5f) 
						aux=0.5f;
					else if (aux>255.5f) 
						aux=255.5f;
					soptr[(numtrans<<1)+1]=(unsigned char)aux;

					numtrans++;
				}
			}

		    glGenTextures(1, &texture_dompos);
			glBindTexture(GL_TEXTURE_2D, texture_dompos);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, transx, transy, 0, GL_LUMINANCE_ALPHA ,GL_UNSIGNED_BYTE, domposptr);

			glGenTextures(1, &texture_ori);
			glBindTexture(GL_TEXTURE_2D, texture_ori);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, transx, transy, 0, GL_LUMINANCE ,GL_UNSIGNED_BYTE, oriptr);

			glGenTextures(1, &texture_so);
			glBindTexture(GL_TEXTURE_2D, texture_so);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, transx, transy, 0, GL_LUMINANCE_ALPHA ,GL_UNSIGNED_BYTE, soptr);

			delete[] domposptr;
			delete[] oriptr;
			delete[] soptr;

		}
		else if (tipofractal==FRACTAL_QUAD)
		{
			cout << "pasame un fractal de tipo const" << endl;
			exit(1);
		}
	}

	filefractalshader.Close();

	glActiveTextureARB(GL_TEXTURE0_ARB);   
	glBindTexture(GL_TEXTURE_2D, texture_dompos);

	glActiveTextureARB(GL_TEXTURE1_ARB);   
	glBindTexture(GL_TEXTURE_2D, texture_so);

	glActiveTextureARB(GL_TEXTURE2_ARB);   
	glBindTexture(GL_TEXTURE_2D, texture_ori);

	shader->begin();
	if (!shader->setUniform1i("texDOMPOS", 0)) std::cout << "No se pudo encontrar texDOMPOS" << std::endl;
	if (!shader->setUniform1i("texSO", 1)) std::cout << "No se pudo encontrar texSO" << std::endl;
	if (!shader->setUniform1i("texORI", 2)) std::cout << "No se pudo encontrar texORI" << std::endl;
	shader->end();

	#endif

	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
	glutDisplayFunc(draw);
	glutKeyboardFunc(key);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(update);
	glutCreateMenu(menu);
	glutAddMenuEntry("jur",0);
	glutAttachMenu(GLUT_RIGHT_BUTTON); 
	
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	//glShadeModel(GL_FLAT);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	//glPointSize(1.0f);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f,1.0f);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);

	glClearColor(1.0f,1.0f,1.0f,1.0f);

	glEnable(GL_LIGHT0);
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -0.5f, 2.0f, -0.5f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
	/*glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);*/
	float specReflection[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	glMateriali(GL_FRONT, GL_SHININESS, 43);
	
	glutMainLoop();

	delete[] rawelev;

	delete fractal;
	delete fr;

	delete[] pooltriangles; 
	delete[] poolpoints; 

	#ifdef FRACTALSHADER
	SM.free(shader);
	#endif

	return 0;
}