#if defined(_MSC_VER)
	#if (_MSC_VER >= 1400)
		#define _CRT_SECURE_NO_DEPRECATE
	#endif
	#include <windows.h>
	#include <io.h>
	#include "../FractalLib/FLFractalConst.h"
	#include "../FractalLib/FLFractalQuad.h"
	#include <GL/glut.h>
	//#include "opengl/glext.h"
	#include "objlib/objlib.h"
	#include "tblib/tblib.h"
#elif defined(__CYGWIN__) || defined(__MINGW32__)
	#include <windows.h>
	#include <io.h>
	#include "FLFractalConst.h"
	#include "FLFractalQuad.h"
	#include <GL/glut.h>
	//#include "opengl/glext.h"
	#include "objlib.h"
	#include "tblib.h"
#else
	#include <sys/time.h>
	#include "FLFractalConst.h"
	#include "FLFractalQuad.h"
	#include <GL/glut.h>
	#include "objlib.h"
	#include "tblib.h"
#endif

#include "MarchingCubes.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
using namespace std;

//YIQ
#define conversion "yiq"
#define XTOR 1.0f
#define XTOG 1.0f
#define XTOB 1.0f
#define YTOR 0.956f
#define YTOG -0.272f
#define YTOB -1.105f
#define ZTOR 0.621f
#define ZTOG -0.647f
#define ZTOB 1.702f

//YUV
/*#define conversion "yuv"
#define XTOR 1.0f
#define XTOG 1.0f
#define XTOB 1.0f
#define YTOR 0.0f
#define YTOG -0.395f
#define YTOB 2.032f
#define ZTOR 1.14f
#define ZTOG -0.581f
#define ZTOB 0.0f*/

char zoomsalida=0;
float precisionbusqueda=0.002f; // medio nivel de gris
char ficherofractal[256]="test.fra2";
char ficheroobj[256];
char ficheroraw[256];
unsigned short tamxentrada;
unsigned short tamyentrada;
unsigned short tamzentrada;
bool color;
unsigned char tipofractal;
unsigned char type; // FRACTAL_2D o FRACTAL_3D
FLFractal2* fractal;
FLFractal3* fractal3;
FLFractal2* fractalxyz[3];
FLFile* file;
FLBitmap2* inibit=NULL;
FLBitmap3* inibit3=NULL;
FLBitmap2* inibitxyz[3];
unsigned char *pixels;
unsigned int nivel=0;
unsigned char capaactual=0;
unsigned int windowx;
unsigned int windowy;

/*int mousepos_x=0;
int mousepos_y=0;
int tmpX,tmpY; */

float coordbeginx=0.0f;
float coordbeginy=0.0f;
float coordendx=1.0f;
float coordendy=1.0f;

float zoom=0.7f;

MarchingCubes* marchingcubes;
MarchingCubes* marchingcubesref;

OLModel objmesh;

bool debugconvexhull=false;
unsigned int marchigcubesdisplaylistbb;
unsigned int referenceobj;
unsigned int referencemarchingobj;

unsigned int lineabarrido;

short VOXELSSIZE=64;

bool render01=false;

unsigned short aplicazoom(unsigned short x)
{
	if (zoomsalida>=0)
		return x<<zoomsalida;
	else
		return x>>(-zoomsalida);
}

unsigned char colorbyte(float x)
{
	if (x>1.0f) return 255;
	if (x<0.0f) return 0;
	return (unsigned char)(x*255.0f+0.5f);
}

long Ticks()
{
#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	return GetTickCount();
#else
	timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000L+tv.tv_usec/1000L;
#endif
}

void reshape(int w,int h)
{
	windowx=w;
	windowy=h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type==FRACTAL_2D)
	{
		glViewport(0,0,w,h);
		gluOrtho2D(0.0f,windowx,windowy,0.0f);
	}
	else if (type==FRACTAL_3D)
	{
		glViewport(0,0,w,h);
		gluPerspective(35.0f,(float)w/(float)h,0.5f,3.5f);
		tbReshape(w/2,h/2);
	}
	else
	{
		glViewport(0,0,w,h);
		gluPerspective(35.0f,(float)w/(float)h,0.5f,3.5f);
		tbReshape(w,h);
	}
	glMatrixMode(GL_MODELVIEW);
}

void draw(void)
{
	if (type==FRACTAL_2D)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		if (render01)
		{

			long tiempo=Ticks();
			
			if (color)
			{
				float colorfx=0.0f; // semilla inicial
				float colorfy=0.0f;
				float colorfz=0.0f;
				unsigned int deep;
				//float deepfx,deepfy,deepfz;
				glBegin(GL_POINTS);
				float x,y;
				//unsigned int windowxdiv2=windowx/2;
				for (unsigned int i=0;i<windowy;i++)
				{
					y=(float)i/(float)(windowy);
					y=coordbeginy+y*(coordendy-coordbeginy);
					for (unsigned int j=0;j<windowx/*div2*/;j++)
					{
						x=(float)j/(float)(windowx/*div2*/);
						x=coordbeginx+x*(coordendx-coordbeginx);
						
						colorfx=fractalxyz[0]->Decode(FLVector2(x,y),precisionbusqueda,colorfx,deep);
						//deepfx=(float)deep*0.05f;
						colorfy=fractalxyz[1]->Decode(FLVector2(x,y),precisionbusqueda,colorfy,deep);
						//deepfy=(float)deep*0.05f;
						colorfz=fractalxyz[2]->Decode(FLVector2(x,y),precisionbusqueda,colorfz,deep);
						//deepfz=(float)deep*0.05f;
						glColor3ub(colorbyte(XTOR*colorfx+YTOR*colorfy+ZTOR*colorfz),
								   colorbyte(XTOG*colorfx+YTOG*colorfy+ZTOG*colorfz),
								   colorbyte(XTOB*colorfx+YTOB*colorfy+ZTOB*colorfz));
						glVertex2i(j,i);		
						/*glColor3ub(colorbyte(deepfx),
								   colorbyte(deepfy),
								   colorbyte(deepfz));
						glVertex2i(j+windowxdiv2,i);*/
						/*if (colorfx<0.0f) colorfx=0.0f;
						if (colorfx>1.0f) colorfx=1.0f;
						if (colorfy<0.0f) colorfy=0.0f;
						if (colorfy>1.0f) colorfy=1.0f;
						if (colorfz<0.0f) colorfz=0.0f;
						if (colorfz>1.0f) colorfz=1.0f;*/
					}
				}
				glEnd();
			}
			else
			{
				float colorf=0.0f; // semilla inicial
				unsigned char colorfub;
				unsigned int deep;
				//float deepf;
				glBegin(GL_POINTS);
				float x,y;
				//unsigned int windowxdiv2=windowx/2;
				for (unsigned int i=0;i<windowy;i++)
				{
					y=(float)i/(float)(windowy);
					y=coordbeginy+y*(coordendy-coordbeginy);
					for (unsigned int j=0;j<windowx/*div2*/;j++)
					{
						x=(float)j/(float)(windowx/*div2*/);
						x=coordbeginx+x*(coordendx-coordbeginx);	

						colorf=fractal->Decode(FLVector2(x,y),precisionbusqueda,colorf,deep);
						colorfub=colorbyte(colorf);
						glColor3ub(colorfub,colorfub,colorfub);
						glVertex2i(j,i);
						/*deepf=(float)deep*0.05f;
						glColor3ub(colorbyte(deepf),
								  colorbyte(deepf),
								  colorbyte(deepf));
						glVertex2i(j+windowxdiv2,i);
						if (colorf<0.0f) colorf=0.0f;
						if (colorf>1.0f) colorf=1.0f;*/
					}
				}
				glEnd();
			}
		    
			tiempo=Ticks()-tiempo;
			/*long horas=tiempo/3600000;
			tiempo-=3600000*horas;
			long minutos=tiempo/60000;
			tiempo-=60000*minutos;
			long segundos=tiempo/1000;
			tiempo-=1000*segundos;
			if (horas) cout << horas << "h ";
			if (minutos) cout << minutos << "m ";
			if (segundos) cout << segundos << "s ";
			if (tiempo)*/ cout << tiempo << "ms";
			cout << endl;
		}
		else
		{
			if (color)
				glDrawPixels(aplicazoom(tamxentrada),aplicazoom(tamyentrada),GL_RGB,GL_UNSIGNED_BYTE,pixels);
			else
				glDrawPixels(aplicazoom(tamxentrada),aplicazoom(tamyentrada),GL_LUMINANCE,GL_UNSIGNED_BYTE,pixels);
		}
	}
	else if (type==FRACTAL_3D)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(-0.75f,0.5f,1.25,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

		tbMatrix();

		glScalef(zoom,zoom,zoom);

		glViewport(0,windowy/2+1,windowx/2,windowy/2);
		glCallList(referenceobj);
		if (debugconvexhull)
		{
			glDisable(GL_LIGHTING);
			glCallList(marchigcubesdisplaylistbb);
			glEnable(GL_LIGHTING);
		}

		glViewport(windowx/2+1,windowy/2+1,windowx/2,windowy/2);
		//glutSolidSphere(0.45f,50,50);
		glCallList(referencemarchingobj);
		if (debugconvexhull)
		{
			glDisable(GL_LIGHTING);
			glCallList(marchigcubesdisplaylistbb);
			glEnable(GL_LIGHTING);
		}
		
		glViewport(0,0,windowx/2,windowy/2);
		marchingcubes->Draw();
		if (debugconvexhull)
		{
			glDisable(GL_LIGHTING);
			glCallList(marchigcubesdisplaylistbb);
			glEnable(GL_LIGHTING);
		}

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glViewport(0,0,windowx,windowy);
		gluOrtho2D(0.0f,windowx,windowy,0.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		float relx=((float)(windowx/2))/(float)(aplicazoom(VOXELSSIZE));
		float rely=((float)(windowy/2))/(float)(aplicazoom(VOXELSSIZE));
		relx=min(relx,rely);
		glPixelZoom(relx,-relx);
		glRasterPos2i(windowx/2+1,windowy/2+1);
		glDrawPixels(aplicazoom(VOXELSSIZE),aplicazoom(VOXELSSIZE),GL_LUMINANCE,GL_FLOAT,inibit3->pixels+lineabarrido*aplicazoom(VOXELSSIZE)*aplicazoom(VOXELSSIZE));
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glViewport(0,0,windowx,windowy);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0f,windowx,windowy,0.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		char buffer[256]="\0";
		glRasterPos2i(5,15);	
		sprintf(buffer,"%d",objmesh.numtriangles);
		char* aux=buffer;
		while(*aux!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
			aux++;
		}
		glRasterPos2i(5+windowx/2,15);	
		sprintf(buffer,"%d",marchingcubesref->numtriangles);
		aux=buffer;
		while(*aux!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
			aux++;
		}
		glRasterPos2i(5,15+windowy/2);	
		sprintf(buffer,"%d",marchingcubes->numtriangles);
		aux=buffer;
		while(*aux!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
			aux++;
		}
		glRasterPos2i(5+windowx/2,15+windowy/2);	
		sprintf(buffer,"%d",lineabarrido);
		aux=buffer;
		while(*aux!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
			aux++;
		}
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		
		glEnable(GL_LIGHTING);		
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(0.0f,0.0f,2.5,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);

		tbMatrix();

		glScalef(zoom,zoom,zoom);

		marchingcubes->Draw();

		glDisable(GL_LIGHTING);
		glViewport(0,0,windowx,windowy);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0f,windowx,windowy,0.0f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		char buffer[256]="\0";
		glRasterPos2i(5,15);	
		sprintf(buffer,"%d",marchingcubes->numtriangles);
		char* aux=buffer;
		while(*aux!=0)
		{
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10,*aux);
			aux++;
		}
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}

	glFlush();
	glutSwapBuffers();
}

void key(unsigned char key, int x, int y)
{
	if (key==27) // escape
	{
		exit(0);
	}
	if (type==FRACTAL_2D)
	{
		if (key=='c') // capa
		{
			if (color)
			{
				if (capaactual<2)
				{
					capaactual++;	
				}
			}
		}
		else if (key==32) // espacio
		{
			FLBitmap2 ret;
			float dif;
			if (color)
			{
				if (capaactual>2)
					return;
				ret=fractalxyz[capaactual]->Decode(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				dif=inibitxyz[capaactual]->DiferenceMean(ret);
				*inibitxyz[capaactual]=ret;
			}
			else
			{
				ret=fractal->Decode(*inibit,zoomsalida);
				dif=inibit->DiferenceMean(ret);
				*inibit=ret;
			}
			nivel++;
			//cout.precision(10);
			cout << "nivel= " << nivel << " dif= " << dif << endl;
			if (color)
			{
				for (unsigned int i=0;i<inibitxyz[0]->numpixels;i++)
				{
					pixels[i*3]=colorbyte((XTOR*inibitxyz[0]->pixels[i])+(YTOR*inibitxyz[1]->pixels[i])+(ZTOR*inibitxyz[2]->pixels[i]));
					pixels[i*3+1]=colorbyte((XTOG*inibitxyz[0]->pixels[i])+(YTOG*inibitxyz[1]->pixels[i])+(ZTOG*inibitxyz[2]->pixels[i]));
					pixels[i*3+2]=colorbyte((XTOB*inibitxyz[0]->pixels[i])+(YTOB*inibitxyz[1]->pixels[i])+(ZTOB*inibitxyz[2]->pixels[i]));
				}
			}
			else
			{
				for (unsigned int i=0;i<inibit->numpixels;i++)
					pixels[i]=colorbyte(inibit->pixels[i]);
			}
			glutPostRedisplay();
		}
		else if (key=='s') // save image
		{
			cout << "guardando imagen..." << endl;
			ofstream salida("test.raw",std::ios_base::binary);
			if (color)
				salida.write((char*)pixels,aplicazoom(tamxentrada)*aplicazoom(tamyentrada)*3);
			else
				salida.write((char*)pixels,aplicazoom(tamxentrada)*aplicazoom(tamyentrada));
		}
		else if (key=='b') // drawbounds
		{
			if (color)
			{
				if (capaactual>2)
					return;
				fractalxyz[capaactual]->DrawBounds(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				for (unsigned int i=0;i<inibitxyz[capaactual]->numpixels;i++)
					pixels[i*3+capaactual]=colorbyte(inibitxyz[capaactual]->pixels[i]);
			
			}
			else
			{
				fractal->DrawBounds(*inibit,zoomsalida);
				for (unsigned int i=0;i<inibit->numpixels;i++)
					pixels[i]=colorbyte(inibit->pixels[i]);
			}
			glutPostRedisplay();
		}
		else if (key=='n') // smoothbounds
		{
			if (color)
			{
				if (capaactual>2)
					return;
				fractalxyz[capaactual]->SmoothBounds(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				for (unsigned int i=0;i<inibitxyz[capaactual]->numpixels;i++)
					pixels[i*3+capaactual]=colorbyte(inibitxyz[capaactual]->pixels[i]);
			}
			else
			{
				fractal->SmoothBounds(*inibit,zoomsalida);
				for (unsigned int i=0;i<inibit->numpixels;i++)
					pixels[i]=colorbyte(inibit->pixels[i]);
			}
			glutPostRedisplay();
		}
		else if (key=='p')
		{
			DWORD ticksantes=Ticks();

			float dif;
			if (color)
			{
				for (;capaactual<3;capaactual++)
				{
					do
					{
						FLBitmap2 ret=fractalxyz[capaactual]->Decode(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
						dif=inibitxyz[capaactual]->DiferenceMean(ret);
						*inibitxyz[capaactual]=ret;
					
						if (color)
						{
							for (unsigned int i=0;i<inibitxyz[0]->numpixels;i++)
							{
								pixels[i*3]=colorbyte((XTOR*inibitxyz[0]->pixels[i])+(YTOR*inibitxyz[1]->pixels[i])+(ZTOR*inibitxyz[2]->pixels[i]));
								pixels[i*3+1]=colorbyte((XTOG*inibitxyz[0]->pixels[i])+(YTOG*inibitxyz[1]->pixels[i])+(ZTOG*inibitxyz[2]->pixels[i]));
								pixels[i*3+2]=colorbyte((XTOB*inibitxyz[0]->pixels[i])+(YTOB*inibitxyz[1]->pixels[i])+(ZTOB*inibitxyz[2]->pixels[i]));
							}
						}
						else
						{
							for (unsigned int i=0;i<inibit->numpixels;i++)
								pixels[i]=colorbyte(inibit->pixels[i]);
						}

						nivel++;
						cout << "nivel= " << nivel << " dif= " << dif << endl;
					}
					while(nivel<100 && dif>precisionbusqueda);
				}
			}
			else
			{
				do
				{
					FLBitmap2 ret=fractal->Decode(*inibit,zoomsalida);
					dif=inibit->DiferenceMean(ret);
					*inibit=ret;
				
					for (unsigned int i=0;i<inibit->numpixels;i++)
						pixels[i]=colorbyte(inibit->pixels[i]);

					nivel++;
					cout << "nivel= " << nivel << " dif= " << dif << endl;
				}
				while(nivel<100 && dif>precisionbusqueda);
			}

			ticksantes=Ticks()-ticksantes;

			cout << "tiempo: " << ticksantes << endl;

			draw();
		}
	}
	else // FRACTAL_3D
	{
		if (key=='c')
		{
			debugconvexhull=!debugconvexhull;
			glutPostRedisplay();
		}
		else if (key==32) // espacio
		{
			FLBitmap3 ret;
			float dif;
			if (color)
			{
				/*if (capaactual>2)
					return;
				ret=fractalxyz[capaactual]->Decode(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				dif=inibitxyz[capaactual]->Diference(ret);
				*inibitxyz[capaactual]=ret;*/
			}
			else
			{
				ret=fractal3->Decode(*inibit3,zoomsalida);
				dif=inibit3->DiferenceMean(ret);
				*inibit3=ret;
			}
			nivel++;
			//cout.precision(10);
			cout << "nivel= " << nivel << " dif= " << dif << endl;
			if (color)
			{
				/*for (unsigned int i=0;i<inibitxyz[0]->numpixels;i++)
				{
					pixels[i*3]=colorbyte((XTOR*inibitxyz[0]->pixels[i])+(YTOR*inibitxyz[1]->pixels[i])+(ZTOR*inibitxyz[2]->pixels[i]));
					pixels[i*3+1]=colorbyte((XTOG*inibitxyz[0]->pixels[i])+(YTOG*inibitxyz[1]->pixels[i])+(ZTOG*inibitxyz[2]->pixels[i]));
					pixels[i*3+2]=colorbyte((XTOB*inibitxyz[0]->pixels[i])+(YTOB*inibitxyz[1]->pixels[i])+(ZTOB*inibitxyz[2]->pixels[i]));
				}*/
			}
			else
			{
				for (unsigned int i=0;i<inibit3->numpixels;i++)
					pixels[i]=colorbyte(inibit3->pixels[i]);
				
				marchingcubes->Load(pixels);
			}
			glutPostRedisplay();
		}
		else if (key=='p')
		{
			DWORD ticksantes=Ticks();

			float dif;
			if (color)
			{
				/*for (;capaactual<3;capaactual++)
				{
					do
					{
						FLBitmap2 ret=fractalxyz[capaactual]->Decode(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
						dif=inibitxyz[capaactual]->Diference(ret);
						*inibitxyz[capaactual]=ret;
					
						if (color)
						{
							for (unsigned int i=0;i<inibitxyz[0]->numpixels;i++)
							{
								pixels[i*3]=colorbyte((XTOR*inibitxyz[0]->pixels[i])+(YTOR*inibitxyz[1]->pixels[i])+(ZTOR*inibitxyz[2]->pixels[i]));
								pixels[i*3+1]=colorbyte((XTOG*inibitxyz[0]->pixels[i])+(YTOG*inibitxyz[1]->pixels[i])+(ZTOG*inibitxyz[2]->pixels[i]));
								pixels[i*3+2]=colorbyte((XTOB*inibitxyz[0]->pixels[i])+(YTOB*inibitxyz[1]->pixels[i])+(ZTOB*inibitxyz[2]->pixels[i]));
							}
						}
						else
						{
							for (unsigned int i=0;i<inibit->numpixels;i++)
								pixels[i]=colorbyte(inibit->pixels[i]);
						}

						nivel++;
						cout << "nivel= " << nivel << " dif= " << dif << endl;
					}
					while(nivel<100 && dif>precisionbusqueda);
				}*/
			}
			else
			{
				do
				{
					FLBitmap3 ret=fractal3->Decode(*inibit3,zoomsalida);
					dif=inibit3->DiferenceMean(ret);
					*inibit3=ret;
				
					for (unsigned int i=0;i<inibit3->numpixels;i++)
						pixels[i]=colorbyte(inibit3->pixels[i]);

					nivel++;
					cout << "nivel= " << nivel << " dif= " << dif << endl;
				}
				while(nivel<100 && dif>precisionbusqueda);
			}

			ticksantes=Ticks()-ticksantes;

			cout << "tiempo: " << ticksantes << endl;

			marchingcubes->Load(pixels);
			draw();
		}
		else if (key=='b') // drawbounds
		{
			if (color)
			{
				/*if (capaactual>2)
					return;
				fractalxyz[capaactual]->DrawBounds(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				for (unsigned int i=0;i<inibitxyz[capaactual]->numpixels;i++)
					pixels[i*3+capaactual]=colorbyte(inibitxyz[capaactual]->pixels[i]);*/
			
			}
			else
			{
				fractal3->DrawBounds(*inibit3,zoomsalida);
				for (unsigned int i=0;i<inibit3->numpixels;i++)
					pixels[i]=colorbyte(inibit3->pixels[i]);
			}
			marchingcubes->Load(pixels);
			glutPostRedisplay();
		}
		else if (key=='n') // smoothbounds
		{
			if (color)
			{
				/*if (capaactual>2)
					return;
				fractalxyz[capaactual]->SmoothBounds(*inibitxyz[capaactual],capaactual==0?zoomsalida:zoomsalida+1);
				for (unsigned int i=0;i<inibitxyz[capaactual]->numpixels;i++)
					pixels[i*3+capaactual]=colorbyte(inibitxyz[capaactual]->pixels[i]);*/
			}
			else
			{
				fractal3->SmoothBounds(*inibit3,zoomsalida);
				for (unsigned int i=0;i<inibit3->numpixels;i++)
					pixels[i]=colorbyte(inibit3->pixels[i]);
			}
			glutPostRedisplay();
		}
		else if (key=='z')
		{
			if (lineabarrido<(unsigned int)(aplicazoom(VOXELSSIZE)-1)) 
			{
				lineabarrido++;
				glutPostRedisplay();
			}
		}
		else if (key=='a')
		{
			if (lineabarrido>0) 
			{
				lineabarrido--;
				glutPostRedisplay();
			}
		}
		else if (key=='+')
		{
			zoom*=1.1f;
			glutPostRedisplay();
		}
		else if (key=='-')
		{
			zoom/=1.1f;
			glutPostRedisplay();
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	if (state==GLUT_DOWN && (button==GLUT_LEFT_BUTTON || button==GLUT_RIGHT_BUTTON))
	{
		if (x>0 && x<(int)(windowx/*/2*/) && y>0 && y<(int)windowy)
		{
			float xf=(float)x/(float)(windowx/*/2*/);
			xf=coordbeginx+xf*(coordendx-coordbeginx);
			float yf=(float)y/(float)windowy;
			yf=coordbeginy+yf*(coordendy-coordbeginy);
			float difx=coordendx-coordbeginx;
			float dify=coordendy-coordbeginy;
			if (button==GLUT_LEFT_BUTTON)
			{
				coordbeginx=xf-difx*0.4f;
				coordbeginy=yf-dify*0.4f;
				coordendx=xf+difx*0.4f;
				coordendy=yf+dify*0.4f;
				if (coordbeginx<0.0f) 
				{
					coordendx-=coordbeginx;
					coordbeginx=0.0f;
				}
				if (coordbeginy<0.0f)
				{
					coordendy-=coordbeginy;
					coordbeginy=0.0f;
				}
				if (coordendx>1.0f) 
				{
					coordbeginx-=(coordendx-1.0f);
					coordendx=1.0f;
				}
				if (coordendy>1.0f)
				{
					coordbeginy-=(coordendy-1.0f);
					coordendy=1.0f;
				}
				glutPostRedisplay();
			}
			else if (button==GLUT_RIGHT_BUTTON)
			{
				coordbeginx=xf-difx*0.6f;
				coordbeginy=yf-dify*0.6f;
				coordendx=xf+difx*0.6f;
				coordendy=yf+dify*0.6f;

				if (coordbeginx<0.0f) 
				{
					coordendx-=coordbeginx;
					coordbeginx=0.0f;
				}
				if (coordbeginy<0.0f)
				{
					coordendy-=coordbeginy;
					coordbeginy=0.0f;
				}
				if (coordendx>1.0f) 
				{
					coordbeginx-=(coordendx-1.0f);
					coordendx=1.0f;
				}
				if (coordendy>1.0f)
				{
					coordbeginy-=(coordendy-1.0f);
					coordendy=1.0f;
				}

				if (coordbeginx<0.0f) coordbeginx=0.0f;
				if (coordbeginy<0.0f) coordbeginy=0.0f;
				if (coordendx>1.0f) coordendx=1.0f;
				if (coordendy>1.0f) coordendy=1.0f;
				glutPostRedisplay();
			}
		}
	}
}

//bool rotate=false;


int empiezax,empiezay;

void mouse3d(int button, int state, int x, int y)
{
	empiezax=(x/(windowx/*/2*/))*(windowx/*/2*/);
	empiezay=(y/(windowy/*/2*/))*(windowy/*/2*/);
	tbMouse(button,state,x-empiezax,y-empiezay);
}

void motion3d(int x, int y)
{
	tbMotion(x-empiezax,y-empiezay);
}

int main(int argc,char **argv)
{
	if (argc>6)
	{
		cout << "Use mode: fractalfile zoom? prec? voxelsize? const?" << endl;
		exit(1);
	}

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
		zoomsalida=atoi(argv[2]);
	}
	if (argc>3)
	{
		precisionbusqueda=atof(argv[3]);
	}
	if (argc>4)
	{
		VOXELSSIZE=atoi(argv[4]);
	}
	if (argc>5)
	{
		render01=strcmp(argv[5],"const")==0;
	}
	
	inibitxyz[0]=inibitxyz[1]=inibitxyz[2];

	FLFile file2(ficherofractal,"r+b");
	file=&file2;

	char clave[8];
	clave[0]=file2.Unpack(8);
	clave[1]=file2.Unpack(8);
	clave[2]=file2.Unpack(8);
	clave[3]=file2.Unpack(8);
	clave[4]=file2.Unpack(8);
	clave[5]=file2.Unpack(8);
	clave[6]=file2.Unpack(8);
	clave[7]='\0';

	if (strcmp("Fractal",clave)!=0)
	{
		/*// imagen raw 256x256x1?

		file2.Close();

		tamxentrada=tamyentrada=256;
		color=0;

		cout << "image:\traw" << endl;
		cout << "zoom:\t" << zoomsalida << endl;
		cout << "file:\t" << ficherofractal << endl;
		cout << "width:\t" << tamxentrada << endl;
		cout << "height:\t" << tamyentrada << endl;
		cout << "color:\t";
		if (color)
			cout << conversion << " -> rgb" << endl;
		else
			cout << "grayscale" << endl;
		cout << endl;

		pixels=new unsigned char[(tamxentrada<<zoomsalida)*(tamyentrada<<zoomsalida)];
		unsigned char* pointer=pixels;
		unsigned char value;

		ifstream rawfile(ficherofractal,ios_base::binary);

		for (unsigned int j=0;j<tamyentrada;j++)
		{
			for (unsigned int i=0;i<tamxentrada;i++)
			{
				value=rawfile.get();
				for (int k=0;k<(1<<zoomsalida);k++)
				{
					*pointer=value;
					pointer++;
				}
			}
			for (int k=0;k<(1<<zoomsalida)-1;k++)
			{
				memcpy(pointer,pointer-(tamxentrada<<zoomsalida),(tamxentrada<<zoomsalida));
				pointer+=(tamxentrada<<zoomsalida);
			}
		}
		
		rawfile.close();

		glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
		glutInitWindowSize(tamxentrada<<zoomsalida,tamyentrada<<zoomsalida);
		glutCreateWindow("FractalView");
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0f,1.0f,1.0f,0.0f);
		glMatrixMode(GL_MODELVIEW);
		glClearColor(1.0f,1.0f,1.0f,1.0f);
		glRasterPos2f(0.0f,0.0f);
		glPixelZoom(1.0f,-1.0f);

		glutReshapeFunc(reshape);
		glutDisplayFunc(draw);
		
		glutMainLoop();

		delete[] pixels;*/

		// imagen raw 3d 256x256x256?

		file2.Close();

		color=0;

		cout << "image:\traw" << endl;
		cout << "zoom:\t" << (short)zoomsalida << endl;
		cout << "file:\t" << ficherofractal << endl;
		cout << "width:\t" << aplicazoom(VOXELSSIZE) << endl;
		cout << "height:\t" << aplicazoom(VOXELSSIZE) << endl;
		cout << "deep:\t" << aplicazoom(VOXELSSIZE) << endl;
		cout << "color:\t";
		if (color)
			cout << conversion << " -> rgb" << endl;
		else
			cout << "grayscale" << endl;
		cout << endl;

		type=0xff;

		#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
		glutInit(&argc,argv);
		#else
		int jg=0;
		glutInit(&jg,NULL);
		#endif

		glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
		glutInitWindowSize(550,700);
		glutCreateWindow("FractalView");
		glutReshapeFunc(reshape);
		glutDisplayFunc(draw);
		glutMouseFunc(mouse3d);
		glutMotionFunc(motion3d);
		glutKeyboardFunc(key);

		tbInit(GLUT_LEFT_BUTTON);
		tbAnimate(GL_TRUE);

		marchigcubesdisplaylistbb=glGenLists(1);
		glNewList(marchigcubesdisplaylistbb,GL_COMPILE);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-0.5f,-0.5f,0.5f);
		glVertex3f(0.5f,-0.5f,0.5f);
		glVertex3f(0.5f,0.5f,0.5f);
		glVertex3f(-0.5f,0.5f,0.5f);
		glVertex3f(-0.5f,-0.5f,0.5f);
		glVertex3f(-0.5f,-0.5f,-0.5f);
		glVertex3f(0.5f,-0.5f,-0.5f);
		glVertex3f(0.5f,0.5f,-0.5f);
		glVertex3f(-0.5f,0.5f,-0.5f);
		glVertex3f(-0.5f,-0.5f,-0.5f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(-0.5f,0.5f,0.5f);
		glVertex3f(-0.5f,0.5f,-0.5f);
		glVertex3f(0.5f,0.5f,0.5f);
		glVertex3f(0.5f,0.5f,-0.5f);
		glVertex3f(0.5f,-0.5f,0.5f);
		glVertex3f(0.5f,-0.5f,-0.5f);
		glEnd();
		glEndList();

		objmesh.Read(ficheroobj);
		//objmesh.Read("bunny.obj");
		//objmesh.Read("cono.obj");
		//objmesh.Read("esfera.obj");
		//objmesh.Read("hand_mini.obj");
		//objmesh.Read("happy_buda_mini.obj");
		objmesh.Weld(0.00001f);
		objmesh.BuildFacetNormals();
		referenceobj=glGenLists(1);
		glNewList(referenceobj,GL_COMPILE);
		objmesh.Draw();
		glEndList();

		glEnable(GL_LIGHTING);
		
		glEnable(GL_LIGHT0);
		GLfloat ambientLight0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat diffuseLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat specularLight0[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		GLfloat position0[] = { -0.3f, 0.3f, 0.9f, 0.0f };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
		glLightfv(GL_LIGHT0, GL_POSITION, position0);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);

		glEnable(GL_LIGHT1);
		GLfloat ambientLight1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat diffuseLight1[] = { 0.6f, 0.6f, 1.0f, 1.0f };
		GLfloat specularLight1[] = { 0.3f, 0.3f, 0.5f, 1.0f };
		GLfloat position1[] = { 0.5f, 0.5f, 0.6f, 0.0f };
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);

		//glEnable(GL_LIGHT2);
		GLfloat ambientLight2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat diffuseLight2[] = { 0.8f, 0.5f, 0.4f, 1.0f };
		GLfloat specularLight2[] = { 0.5f, 0.4f, 0.3f, 1.0f };
		GLfloat position2[] = { 0.3f, -0.3f, -0.2f, 0.0f };
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight2);
		glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight2);
		glLightfv(GL_LIGHT2, GL_POSITION, position2);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180.0);
		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
		
		float specReflection[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, specReflection);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
		glMateriali(GL_FRONT, GL_SHININESS, 35);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//glShadeModel(GL_FLAT);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f,1.0f);

		glEnable(0x803A);//GL_RESCALE_NORMAL);
		
		marchingcubes=new MarchingCubes(aplicazoom(VOXELSSIZE),ficherofractal);

		glutMainLoop();

		glDeleteLists(marchigcubesdisplaylistbb,1);
		glDeleteLists(referenceobj,1);
		
		delete marchingcubes;
	}
	else
	{
		unsigned char version;
		version=file2.Unpack(8);
		type=file2.Unpack(8);
		cout << "image:\tfractal" << endl;
		cout << "zoom:\t" << (short)zoomsalida << endl;
		cout << "prec:\t" << precisionbusqueda << endl;
		cout << "file:\t" << ficherofractal << endl;
		cout << "key:\t" << clave << endl;
		cout << "rev:\t" << (int)version << endl;
		if (type==0)
			cout << "type:\t2d" << endl;
		else
			cout << "type:\t3d" << endl;
		if (type==FRACTAL_2D)
		{
			tamxentrada=file2.Unpack(16);
			tamyentrada=file2.Unpack(16);
			color=(file2.Unpack(1)==1);
	
			cout << "width:\t" << tamxentrada << endl;
			cout << "height:\t" << tamyentrada << endl;
			cout << "color:\t";
			if (color)
				cout << conversion << " -> rgb" << endl;
			else
				cout << "grayscale" << endl;
			cout << endl;

			if (color)
			{
				for (unsigned int jh=0;jh<3;jh++)
				{
					tipofractal=(unsigned char)file2.Unpack(8);
					if (tipofractal==FRACTAL_CONST)
						fractalxyz[jh]=new FLFractalConst2();
					else if (tipofractal==FRACTAL_QUAD)
						fractalxyz[jh]=new FLFractalQuad2();
					fractalxyz[jh]->imagetamx=jh==0?tamxentrada:tamxentrada/2;
					fractalxyz[jh]->imagetamy=jh==0?tamyentrada:tamyentrada/2;
					fractalxyz[jh]->Load(file2);
				}
			}
			else
			{
				tipofractal=(unsigned char)file2.Unpack(8);
				if (tipofractal==FRACTAL_CONST)
					fractal=new FLFractalConst2();
				else if (tipofractal==FRACTAL_QUAD)
					fractal=new FLFractalQuad2();
				fractal->imagetamx=tamxentrada;
				fractal->imagetamy=tamyentrada;
				fractal->Load(file2);
			}
			
			if(!render01)
			{
				if (color)
				{
					pixels=new unsigned char[aplicazoom(tamxentrada)*aplicazoom(tamyentrada)*3];
					memset(pixels,0,aplicazoom(tamxentrada)*aplicazoom(tamyentrada)*3);
				}
				else
				{
					pixels=new unsigned char[aplicazoom(tamxentrada)*aplicazoom(tamyentrada)];
					memset(pixels,0,aplicazoom(tamxentrada)*aplicazoom(tamyentrada));
				}
			
				if (color)
				{
					inibitxyz[0]=new FLBitmap2(aplicazoom(tamxentrada),aplicazoom(tamyentrada)); // bitmap de representación
					/*for (unsigned int i=0;i<inibitxyz[0]->numpixels;i++)
						inibitxyz[0]->pixels[i]=1.0f;*/
					inibitxyz[1]=new FLBitmap2(aplicazoom(tamxentrada),aplicazoom(tamyentrada)); // bitmap de representación
					/*for (unsigned int i=0;i<inibitxyz[1]->numpixels;i++)
						inibitxyz[1]->pixels[i]=0.0f;*/
					inibitxyz[2]=new FLBitmap2(aplicazoom(tamxentrada),aplicazoom(tamyentrada)); // bitmap de representación
					/*for (unsigned int i=0;i<inibitxyz[2]->numpixels;i++)
						inibitxyz[2]->pixels[i]=0.0f;*/
				}
				else
				{
					inibit=new FLBitmap2(aplicazoom(tamxentrada),aplicazoom(tamyentrada)); // bitmap de representación
				}
			}
			else
			{
				// si x o y se pasan de 512 ajustar
				float to512x=512.0f/(float)tamxentrada;
				float to512y=512.0f/(float)tamyentrada;
				if (to512x<1.0f || to512y<1.0f)
				{
					if (to512y<to512x)
						to512x=to512y;
					tamxentrada=(unsigned short)((float)tamxentrada*to512x);
					tamyentrada=(unsigned short)((float)tamyentrada*to512x);
				}
			}

			windowx=tamxentrada;
			windowy=tamyentrada;	
			
			#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
			glutInit(&argc,argv);
			#else
			int jg=0;
			glutInit(&jg,NULL);
			#endif
				
			if(render01)
			{
				glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
				glutInitWindowSize(aplicazoom(tamxentrada)/**2*/,aplicazoom(tamyentrada));
				glutCreateWindow("FractalView");
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(0.0f,aplicazoom(tamxentrada)/**2*/,aplicazoom(tamyentrada),0.0f);
				glMatrixMode(GL_MODELVIEW);
				glClearColor(1.0f,1.0f,1.0f,1.0f);
				glRasterPos2f(0.0f,0.0f);
				glPixelZoom(1.0f,-1.0f);
				glutMouseFunc(mouse);
			}
			else
			{
				glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
				glutInitWindowSize(aplicazoom(tamxentrada),aplicazoom(tamyentrada));
				glutCreateWindow("FractalView");
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluOrtho2D(0.0f,1.0f,1.0f,0.0f);
				glMatrixMode(GL_MODELVIEW);
				glClearColor(1.0f,1.0f,1.0f,1.0f);
				glRasterPos2f(0.0f,0.0f);
				glPixelZoom(1.0f,-1.0f);
			}

			glutReshapeFunc(reshape);
			glutDisplayFunc(draw);
			glutKeyboardFunc(key);
			
			glutMainLoop();

			if (color)
			{
				delete fractalxyz[0];
				delete fractalxyz[1];
				delete fractalxyz[2];
			}
			else
			{
				delete fractal;
			}

			if (!render01)
			{
				delete[] pixels;
				if (inibit)
					delete inibit;
				if (inibitxyz[0])
					delete inibitxyz[0];
				if (inibitxyz[1])
					delete inibitxyz[1];
				if (inibitxyz[2])
					delete inibitxyz[2];
			}
		}
		else //FRACTAL_3D
		{
			memcpy(ficheroraw,ficherofractal,strlen(ficherofractal)-5);
			sprintf(ficheroobj,"%s.obj",ficheroraw);
			sprintf(ficheroraw,"%s.raw",ficheroraw);

			tamxentrada=file2.Unpack(16);
			tamyentrada=file2.Unpack(16);
			tamzentrada=file2.Unpack(16);
			color=(file2.Unpack(1)==1);
		
			cout << "width:\t" << tamxentrada << endl;
			cout << "height:\t" << tamyentrada << endl;
			cout << "deep:\t" << tamzentrada << endl;
			cout << "color:\t";
			if (color)
				cout << conversion << " -> rgb" << endl;
			else
				cout << "grayscale" << endl;
			cout << endl;

			if (color)
			{
			}
			else
			{
				tipofractal=(unsigned char)file2.Unpack(8);
				if (tipofractal==FRACTAL_CONST)
					fractal3=new FLFractalConst3();
				else if (tipofractal==FRACTAL_QUAD)
					fractal3=new FLFractalQuad3();
				fractal3->imagetamx=tamxentrada;
				fractal3->imagetamy=tamyentrada;
				fractal3->imagetamz=tamzentrada;
				fractal3->Load(file2);

				inibit3=new FLBitmap3(aplicazoom(tamxentrada),aplicazoom(tamyentrada),aplicazoom(tamzentrada)); // bitmap de representación
			
				#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
				glutInit(&argc,argv);
				#else
				int jg=0;
				glutInit(&jg,NULL);
				#endif

				windowx=800;
				windowy=800;

				lineabarrido=(aplicazoom(VOXELSSIZE)>>1)-1;

				glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
				glutInitWindowSize(windowx,windowy);
				glutCreateWindow("FractalView");
				glutReshapeFunc(reshape);
				glutDisplayFunc(draw);
				glutMouseFunc(mouse3d);
				glutMotionFunc(motion3d);
				glutKeyboardFunc(key);

				tbInit(GLUT_LEFT_BUTTON);
				tbAnimate(GL_TRUE);

				glEnable(0x803A);//GL_RESCALE_NORMAL);

				marchigcubesdisplaylistbb=glGenLists(1);
				glNewList(marchigcubesdisplaylistbb,GL_COMPILE);
				glBegin(GL_LINE_LOOP);
				glVertex3f(-0.5f,-0.5f,0.5f);
				glVertex3f(0.5f,-0.5f,0.5f);
				glVertex3f(0.5f,0.5f,0.5f);
				glVertex3f(-0.5f,0.5f,0.5f);
				glVertex3f(-0.5f,-0.5f,0.5f);
				glVertex3f(-0.5f,-0.5f,-0.5f);
				glVertex3f(0.5f,-0.5f,-0.5f);
				glVertex3f(0.5f,0.5f,-0.5f);
				glVertex3f(-0.5f,0.5f,-0.5f);
				glVertex3f(-0.5f,-0.5f,-0.5f);
				glEnd();
				glBegin(GL_LINES);
				glVertex3f(-0.5f,0.5f,0.5f);
				glVertex3f(-0.5f,0.5f,-0.5f);
				glVertex3f(0.5f,0.5f,0.5f);
				glVertex3f(0.5f,0.5f,-0.5f);
				glVertex3f(0.5f,-0.5f,0.5f);
				glVertex3f(0.5f,-0.5f,-0.5f);
				glEnd();
				glEndList();

				objmesh.Read(ficheroobj);
				//objmesh.Read("bunny.obj");
				//objmesh.Read("cono.obj");
				//objmesh.Read("esfera.obj");
				//objmesh.Read("hand_mini.obj");
				//objmesh.Read("happy_buda_mini.obj");
				objmesh.Weld(0.00001f);
				objmesh.BuildFacetNormals();
				referenceobj=glGenLists(1);
				glNewList(referenceobj,GL_COMPILE);
				objmesh.Draw();
				glEndList();

				marchingcubesref=new MarchingCubes(VOXELSSIZE,ficheroraw);
				referencemarchingobj=glGenLists(1);
				glNewList(referencemarchingobj,GL_COMPILE);
				marchingcubesref->Draw();
				glEndList();

				glEnable(GL_LIGHTING);
				
				glEnable(GL_LIGHT0);
				GLfloat ambientLight0[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat diffuseLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat specularLight0[] = { 0.5f, 0.5f, 0.5f, 1.0f };
				GLfloat position0[] = { -0.3f, 0.3f, 0.9f, 0.0f };
				glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
				glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);
				glLightfv(GL_LIGHT0, GL_POSITION, position0);
				glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
				glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
				glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);

				glEnable(GL_LIGHT1);
				GLfloat ambientLight1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat diffuseLight1[] = { 0.6f, 0.6f, 1.0f, 1.0f };
				GLfloat specularLight1[] = { 0.3f, 0.3f, 0.5f, 1.0f };
				GLfloat position1[] = { 0.5f, 0.5f, 0.6f, 0.0f };
				glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
				glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
				glLightfv(GL_LIGHT1, GL_POSITION, position1);
				glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0);
				glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
				glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);

				//glEnable(GL_LIGHT2);
				GLfloat ambientLight2[] = { 0.0f, 0.0f, 0.0f, 1.0f };
				GLfloat diffuseLight2[] = { 0.8f, 0.5f, 0.4f, 1.0f };
				GLfloat specularLight2[] = { 0.5f, 0.4f, 0.3f, 1.0f };
				GLfloat position2[] = { 0.3f, -0.3f, -0.2f, 0.0f };
				glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight2);
				glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight2);
				glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight2);
				glLightfv(GL_LIGHT2, GL_POSITION, position2);
				glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180.0);
				glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);
				glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0);
				
				float specReflection[] = { 0.5f, 0.5f, 0.5f, 1.0f };
				glMaterialfv(GL_FRONT, GL_DIFFUSE, specReflection);
				glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
				glMateriali(GL_FRONT, GL_SHININESS, 35);

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
				//glShadeModel(GL_FLAT);
				//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);

				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f,1.0f);
				
				marchingcubes=new MarchingCubes(aplicazoom(VOXELSSIZE));
				pixels=new unsigned char[inibit3->numpixels];

				if (render01)
				{
					unsigned int deep;
					float fi,fj,fk;
					float valorantes=0.0f;
					unsigned short txentrada=aplicazoom(tamxentrada);
					unsigned short tyentrada=aplicazoom(tamyentrada);
					unsigned short tzentrada=aplicazoom(tamzentrada);
					unsigned int aux1=0;
					unsigned int aux2=txentrada*tyentrada;
					unsigned int aux3=0;

					long tiempo=Ticks();

					for (unsigned int k=0;k<tzentrada;k++)
					{
						fk=(float)k/(float)(tzentrada-1);
						for (unsigned int j=0;j<tyentrada;j++)
						{
							fj=(float)j/(float)(tyentrada-1);
							for (unsigned int i=0;i<txentrada;i++)
							{
								fi=(float)i/(float)(txentrada-1);
								valorantes=fractal3->Decode(FLVector3(fi,fj,fk),precisionbusqueda,valorantes,deep);
								inibit3->pixels[aux1+aux3+i]=valorantes;
								pixels[aux1+aux3+i]=colorbyte(valorantes);
							}
							aux3+=txentrada;
						}
						aux1+=aux2;
						aux3=0;
					}

					tiempo=Ticks()-tiempo;
					cout << tiempo << "ms";
					cout << endl;

					marchingcubes->Load(pixels);
					/*ofstream file("test.raw",ios_base::binary);
					file.write((char*)pixels,tamxentrada*tamyentrada*tamzentrada);
					file.close();*/
				}

				glutMainLoop();

				glDeleteLists(marchigcubesdisplaylistbb,1);
				glDeleteLists(referenceobj,1);
				glDeleteLists(referencemarchingobj,1);

				delete marchingcubes;
				delete marchingcubesref;
				delete fractal3;
				delete inibit3;
				delete[] pixels;
			}
		}
	}

	return 0;
}