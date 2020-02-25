// ************************************************
// Tutorial 6: Textures
//
// (c) 2003 by Martin Christen.
// ************************************************

#include "glsl/glsl.h"
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include "../FractalLib/FLFile.h"

using namespace std;

#define FRACTAL_CONST	0
#define FRACTAL_QUAD	1

#define FRAG_FILE "fractal.frag"
#define VERT_FILE "fractal.vert"
#define RAW_FILE "tile1-256.raw"
#define FRACTAL_FILE "tile1-256.fra2"

glShaderManager SM;
glShader* shader=NULL;

GLuint texture_dompos;
GLuint texture_ori;
GLuint texture_so;
GLuint texture_lena;

static int mousepos_x = 0, mousepos_y = 0, tmpX, tmpY; 
bool rotate=false;

bool _use_shader = true;

float camera[3];
float lookat[3];
float cam[3];

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Modelview Transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(camera[0], camera[1], camera[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);
    
    //glTranslatef(0.0,0.0,-5.0);

    glTranslatef(cam[0],cam[1],cam[2]);

    // Simple mouse rotation:
    glRotatef((GLfloat) mousepos_x, -1.0, 0.0, 0.0);
    glRotatef((GLfloat) mousepos_y, 0.0, -1.0, 0.0);

   shader->begin();

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
   shader->end();

   glutSwapBuffers();
    
}

void glinit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glShadeModel(GL_FLAT);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS); 
    //glEnable(GL_CULL_FACE);

	glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.5f, 1.5f, 2.5f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	float specReflection[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	glMateriali(GL_FRONT, GL_SHININESS, 43);
}

void reshape(int w, int h)
{
float aspect = (float) w/((h)?h:1);

    glViewport(0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
                      
    gluPerspective(45.0f, aspect, 0.1f, 10.0f);
    glViewport(0,0,w,h);                      

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{                             
    switch (key)
    {
        case 27:    // escape: exit
			SM.free(shader);
            exit(0);
            break;
            
        case 'w':
            cam[1] += 0.1;
            break;   
            
        case 's':
            cam[1] -= 0.1;
            break;   
            
        case 'a':
            cam[0] += 0.1;
            break;   
            
        case 'd':
            cam[0] -= 0.1;
            break;
        
       case 'r':
            cam[2] += 0.1;
            break; 
                  
        case 'f':
            cam[2] -= 0.1;
            break;          
                  

        case 'p':   // turn Shader on/off
            _use_shader = !_use_shader;
			_use_shader?shader->enable():shader->disable();
            break;

    }

	glutPostRedisplay();
    
}

void mouse(int button, int state, int x, int z)
{
 switch(button){
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) 
    {
      tmpX = x;
      tmpY = z;
      rotate = true;
    } 
    else 
    {
      rotate = false;
    }
    break;
  }
  

}

void motion(int x, int z)
{
  if(rotate)   //Rotiere, wenn linke maustaste gedrückt
  {
    mousepos_y = (mousepos_y + (x - tmpX))%360;
    mousepos_x = (mousepos_x + (z - tmpY))%360;
    tmpX = x;
    tmpY = z;
    glutPostRedisplay();
  }

}

void main(int argc, char* argv[])
{ 
	camera[0] = 0.0f;
	camera[1] = 0.0f;
	camera[2] = 5.0f;

	cam[0] = cam[1] = cam[2] = 0.0f;

	lookat[0] = 0.0f-camera[0];
	lookat[1] = 0.0f-camera[1];
	lookat[2] = 5.0f-camera[2];


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);
	
	glinit();	

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(specialkeys);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//glutIdleFunc(animate);

	checkGL2();
	shader = SM.loadfromFile(VERT_FILE, FRAG_FILE);
	
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

			unsigned char* domposptr=new unsigned char[numtransformations*2];
			memset(domposptr,255,numtransformations*2);
			unsigned char* oriptr=new unsigned char[numtransformations*1];		
			memset(oriptr,255,numtransformations*1);
			unsigned char* soptr=new unsigned char[numtransformations*2];
			memset(soptr,255,numtransformations*2);

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

					domposptr[numtrans*2]=domposx;
					domposptr[(numtrans*2)+1]=domposy;
					oriptr[numtrans*1]=(ori<<5);
					soptr[numtrans*2]=(unsigned char)(((s*0.5f)+0.5f)*255.0f+0.5f);
					
					float aux=(o*0.5f)+127.5f+0.5f;
					if (aux<0.5f) 
						aux=0.5f;
					else if (aux>255.5f) 
						aux=255.5f;
					soptr[(numtrans*2)+1]=(unsigned char)aux;

					numtrans++;
				}
			}

			/*ofstream file_dompos("fractalshader-dompos.raw",ios_base::binary);
			file_dompos.write((char*)domposptr,numtransformations*2);
			file_dompos.close();
			ofstream file_ori("fractalshader-ori.raw",ios_base::binary);
			file_ori.write((char*)oriptr,numtransformations*1);
			file_ori.close();
			ofstream file_so("fractalshader-so.raw",ios_base::binary);
			file_so.write((char*)soptr,numtransformations*2);
			file_so.close();*/

			/*ifstream file_dompos("fractalshader-dompos.raw",ios_base::binary);
			file_dompos.read((char*)domposptr,transx*transy*2);
			file_dompos.close();
			ifstream file_ori("fractalshader-ori.raw",ios_base::binary);
			file_ori.read((char*)oriptr,transx*transy*1);
			file_ori.close();
			ifstream file_so("fractalshader-so.raw",ios_base::binary);
			file_so.read((char*)soptr,transx*transy*2);
			file_so.close();*/

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

   shader->begin();
   if (!shader->setUniform1i("texDOMPOS", 1)) std::cout << "No se pudo encontrar texDOMPOS" << std::endl;
   if (!shader->setUniform1i("texSO", 2)) std::cout << "No se pudo encontrar texSO" << std::endl;
   if (!shader->setUniform1i("texORI", 3)) std::cout << "No se pudo encontrar texORI" << std::endl;
   shader->end();

   glutMainLoop();

   SM.free(shader);
}