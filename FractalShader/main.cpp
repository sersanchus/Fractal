// ************************************************
// Tutorial 6: Textures
//
// (c) 2003 by Martin Christen.
// ************************************************

#include "aGLSL/aGLSL.h"
#include "simpleGL.h"
#include <GL/glut.h>
#include <iostream>
#include <math.h>

#include "../FractalLib/FLFile.h"

using namespace std;

#define FRAG_FILE "fractal.frag"
#define VERT_FILE "fractal.vert"
#define RAW_FILE "tile1-256.raw"
#define FRACTAL_FILE "tile1-256.fra2"


// GLobals:
aShaderObject* myShader = 0;  
GLuint texture_dompos;
GLuint texture_ori;
GLuint texture_so;

GLuint texture_lena;

//*************************************************************
//Draw one Frame (don't Swap Buffers)
void DrawFrame(void)
{
   myShader->begin();

   //glutSolidTeapot(1.0);
   /*glScalef(2.0, 2.0, 2.0);
   glBegin(GL_QUADS);
     glTexCoord2f(0.0f,0.0f);
     glVertex2f(-1.0f, 1.0f);

	 glTexCoord2f(0.0f,1.0f);
	 glVertex2f(-1.0f,-1.0f);
	
	 glTexCoord2f(1.0f,1.0f);
	 glVertex2f( 1.0f,-1.0f);

	 glTexCoord2f(1.0f,0.0f);
	 glVertex2f(1.0f,1.0f);
   glEnd();*/
   
	/*static GLfloat xplane[4] = { 1, 0, 0, 0 };
	static GLfloat zplane[4] = { 0, 1, 0, 0 };

	glTexGenfv(GL_S, GL_OBJECT_PLANE, xplane);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, zplane);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glutSolidCube(1.5f);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);*/

   glBegin(GL_QUADS);
	 glNormal3f(0.0f,0.0f,1.0f);
     glTexCoord2f(0.0f,0.0f);
     glVertex3f(-1.0f,1.0f,1.0f);
	 glNormal3f(0.0f,0.0f,1.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(-1.0f,-1.0f,1.0f);	
	 glNormal3f(0.0f,0.0f,1.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(1.0f,-1.0f,1.0f);
	 glNormal3f(0.0f,0.0f,1.0f);
	 glTexCoord2f(1.0f,0.0f);
	 glVertex3f(1.0f,1.0f,1.0f);

	 glNormal3f(0.0f,0.0f,-1.0f);
	 glTexCoord2f(1.0f,0.0f);
     glVertex3f(-1.0f,1.0f,-1.0f);
	 glNormal3f(0.0f,0.0f,-1.0f);
	 glTexCoord2f(0.0f,0.0f);
	 glVertex3f(1.0f,1.0f,-1.0f);
	 glNormal3f(0.0f,0.0f,-1.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(1.0f,-1.0f,-1.0f);
	 glNormal3f(0.0f,0.0f,-1.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(-1.0f,-1.0f,-1.0f);

	 glNormal3f(1.0f,0.0f,0.0f);
	 glTexCoord2f(0.0f,0.0f);
     glVertex3f(1.0f,1.0f,1.0f);
	 glNormal3f(1.0f,0.0f,0.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(1.0f,-1.0f,1.0f);	
	 glNormal3f(1.0f,0.0f,0.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(1.0f,-1.0f,-1.0f);
	 glNormal3f(1.0f,0.0f,0.0f);
	 glTexCoord2f(1.0f,0.0f);
	 glVertex3f(1.0f,1.0f,-1.0f);

	 glNormal3f(-1.0f,0.0f,0.0f);
	 glTexCoord2f(0.0f,0.0f);
     glVertex3f(-1.0f,1.0f,-1.0f);
	 glNormal3f(-1.0f,0.0f,0.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(-1.0f,-1.0f,-1.0f);	
	 glNormal3f(-1.0f,0.0f,0.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(-1.0f,-1.0f,1.0f);
	 glNormal3f(-1.0f,0.0f,0.0f);
	 glTexCoord2f(1.0f,0.0f);
	 glVertex3f(-1.0f,1.0f,1.0f);

	 glNormal3f(0.0f,1.0f,0.0f);
	 glTexCoord2f(0.0f,0.0f);
     glVertex3f(-1.0f,1.0f,-1.0f);
	 glNormal3f(0.0f,1.0f,0.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(-1.0f,1.0f,1.0f);	
	 glNormal3f(0.0f,1.0f,0.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(1.0f,1.0f,1.0f);
	 glNormal3f(0.0f,1.0f,0.0f);
	 glTexCoord2f(1.0f,0.0f);
	 glVertex3f(1.0f,1.0f,-1.0f);

	 glNormal3f(0.0f,-1.0f,0.0f);
	 glTexCoord2f(0.0f,0.0f);
     glVertex3f(-1.0f,-1.0f,1.0f);
	 glNormal3f(0.0f,-1.0f,0.0f);
	 glTexCoord2f(0.0f,1.0f);
	 glVertex3f(-1.0f,-1.0f,-1.0f);	
	 glNormal3f(0.0f,-1.0f,0.0f);
	 glTexCoord2f(1.0f,1.0f);
	 glVertex3f(1.0f,-1.0f,-1.0f);
	 glNormal3f(0.0f,-1.0f,0.0f);
	 glTexCoord2f(1.0f,0.0f);
	 glVertex3f(1.0f,-1.0f,1.0f);
	glEnd();

/*   glTranslatef(2.0,0.0,0.0);
   glBindTexture(GL_TEXTURE_2D, texture2);
   glutSolidTeapot(0.5);*/
   myShader->end();
    
}

#define FRACTAL_CONST	0
#define FRACTAL_QUAD	1

void AppInit(void)
{ 
	myShader = DefaultShaderSetup(VERT_FILE, FRAG_FILE);
	
	glEnable(GL_TEXTURE_2D);

	glDepthFunc(GL_LESS); 
    glEnable(GL_CULL_FACE);
 

	// leo textura original

	char* lenaptr=new char[256*256*1];
	std::ifstream filelena(RAW_FILE,std::ios_base::binary);
	filelena.read(lenaptr,256*256*1);
	filelena.close();
	glGenTextures(1, &texture_lena);
	glBindTexture(GL_TEXTURE_2D, texture_lena);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 256, 0, GL_LUMINANCE ,GL_UNSIGNED_BYTE, lenaptr);
	delete[] lenaptr;

	// leo fractal

	FLFile file(FRACTAL_FILE,"r+b");

	char clave[8];
	clave[0]=file.Unpack(8);
	clave[1]=file.Unpack(8);
	clave[2]=file.Unpack(8);
	clave[3]=file.Unpack(8);
	clave[4]=file.Unpack(8);
	clave[5]=file.Unpack(8);
	clave[6]=file.Unpack(8);
	clave[7]='\0';

	if (strcmp("Fractal",clave)!=0)
	{
		cout << FRACTAL_FILE << " isn't a fractal" << endl;
		exit(1);
	}

	unsigned char version;
	version=file.Unpack(8);
	unsigned char type;
	type=file.Unpack(8);
	unsigned short tamxentrada=file.Unpack(16);
	unsigned short tamyentrada=file.Unpack(16);
	
	if (tamxentrada>256 || tamyentrada>256)
	{
		cout << "pasame un fractal de 256 de tamaño" << endl;
		exit(1);
	}	
	
	bool color=(file.Unpack(1)==1);

	cout << "file:\t" << FRACTAL_FILE << endl;
	cout << "key:\t" << clave << endl;
	cout << "rev:\t" << (int)version << endl;
	if (type==0)
		cout << "type:\t2d" << endl;
	else
		cout << "type:\t3d" << endl;
	cout << "width:\t" << tamxentrada << endl;
	cout << "height:\t" << tamyentrada << endl;
	cout << "type:\t";
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
		unsigned char tipofractal=(unsigned char)file.Unpack(8);
		if (tipofractal==FRACTAL_CONST)
		{
			unsigned short ransize=(unsigned short)file.Unpack(16);

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

					unsigned char pis=(unsigned char)file.Unpack(5);
					unsigned char pio=(unsigned char)file.Unpack(7);
					if (pis!=16)
					{
						domposx=(unsigned short)file.Unpack(16);
						domposy=(unsigned short)file.Unpack(16);
						ori=(unsigned char)file.Unpack(3);	
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

	file.Close();


  // ---------------------------------------
  // OpenGL Settings
  
  //DefaultLighting();

  //texture_dompos=LoadTextureDOMPOS("fractalshader-dompos.raw",64);
  //texture_ori=LoadTextureORI("fractalshader-ori.raw",64);
  //texture_so=LoadTextureSO("fractalshader-so.raw",64);

  // LoadTexture is definied in simpleGL.cpp, it
  // loads a 256x256 RAW RGBA (32bit) texture.
/*  unsigned * tex = LoadTexture1UB("../textures/texture256x256RGBA.raw");
  char* tex2 = LoadTexture2F("../textures/texture2-256x256RGBA.raw");
  char* tex2 = LoadTexture2US("../textures/texture2-256x256RGBA.raw");
  
  // createTexture creates an OpenGL 2D Texture (defined in
  // simpleGL.cpp)

  createTexture(tex, &texture1);
  createTexture(tex2, &texture2);*/

   glActiveTextureARB(GL_TEXTURE0_ARB);   
   glBindTexture(GL_TEXTURE_2D, texture_lena);

   glActiveTextureARB(GL_TEXTURE1_ARB);   
   glBindTexture(GL_TEXTURE_2D, texture_dompos);

   glActiveTextureARB(GL_TEXTURE2_ARB);   
   glBindTexture(GL_TEXTURE_2D, texture_so);

   glActiveTextureARB(GL_TEXTURE3_ARB);   
   glBindTexture(GL_TEXTURE_2D, texture_ori);

   myShader->begin();
   if (!myShader->sendUniform1i("texDOMPOS", 1)) std::cout << "No se pudo encontrar texDOMPOS" << std::endl;
   if (!myShader->sendUniform1i("texSO", 2)) std::cout << "No se pudo encontrar texSO" << std::endl;
   if (!myShader->sendUniform1i("texORI", 3)) std::cout << "No se pudo encontrar texORI" << std::endl;
   myShader->end();
  
}

//*************************************************************
// use App Exit to clean up
void AppExit(void)
{
  DefaultCleanup();
}

//*************************************************************
//END