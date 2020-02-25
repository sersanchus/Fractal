#include <windows.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include "../FractalLib/FLFractalConst.h"
#include "../FractalLib/FLBitmap.h"
#include "../FractalLib/FLFile.h"
#include "objlib/objlib.h"
#include "Octree.h"

using namespace std;

#define VOXELSSIZE 128

void main(void)
{
	#define BUDA
	
	unsigned long ticks, ticksantes;

	//////////////////////////
	// CARGAMOS LA MALLA OBJ
	//////////////////////////

	#ifdef BUDA

	#define OBJFILE "dragon.obj"
	//#define OBJFILE "bunny.obj"
	//#define OBJFILE "cono.obj"
	//#define OBJFILE "esfera.obj"
	//#define OBJFILE "hand_mini.obj"
	//#define OBJFILE "happy_buda_mini.obj"
	OLModel objmesh;
	//objmesh.Read("test.obj");
	cout << "Loading triangle mesh " << OBJFILE << "... ";

	ticksantes=GetTickCount();
	objmesh.Read(OBJFILE);
	objmesh.Weld(0.00001f);
	objmesh.BuildFacetNormals();
	objmesh.BuildVertexNormals(180.0f);
	ticks=GetTickCount();
	cout << ticks-ticksantes << " ms" << endl;

	//////////////////////////
	// CONSTRUIMOS OCTREE
	//////////////////////////

	cout << "Octree reordering" << "... ";

	ticksantes=GetTickCount();

	float desp=(1.0f/((float)VOXELSSIZE-1.0f))*0.5f;
	Octree octree(-0.5f-desp,-0.5f-desp,-0.5f-desp,0.5f+desp,0.5f+desp,0.5f+desp);
	TriangleNode* aux;
	
	Triangle* mintri=NULL;
	float mindist=99999999999.9f;
	float distaux;
	
	for (unsigned int i=0;i<objmesh.numtriangles;i++)
	{
		aux=new TriangleNode;
		aux->t=new Triangle;
		memcpy(&aux->t->v[0],&objmesh.vertices[objmesh.triangles[i].vindices[0]*3],3*sizeof(float));
		memcpy(&aux->t->v[1],&objmesh.vertices[objmesh.triangles[i].vindices[1]*3],3*sizeof(float));
		memcpy(&aux->t->v[2],&objmesh.vertices[objmesh.triangles[i].vindices[2]*3],3*sizeof(float));
		
		if (BoxTriangleIntersect(octree.root->boxpoints,aux->t))
		{
			memcpy(&aux->t->fn,&objmesh.facetnorms[objmesh.triangles[i].findex*3],3*sizeof(float));
			memcpy(&aux->t->n[0],&objmesh.normals[objmesh.triangles[i].nindices[0]*3],3*sizeof(float));
			memcpy(&aux->t->n[1],&objmesh.normals[objmesh.triangles[i].nindices[1]*3],3*sizeof(float));
			memcpy(&aux->t->n[2],&objmesh.normals[objmesh.triangles[i].nindices[2]*3],3*sizeof(float));

			// calculo la normal que tendrá cada una de las aristas del triángulo
			// a partir de las normales de cara de los triángulos que la compartan
			for (unsigned int aristai=0;aristai<3;aristai++)
			{
				bool compartida=false;
				unsigned int aristai0=objmesh.triangles[i].vindices[aristai];
				unsigned int aristai1=objmesh.triangles[i].vindices[(aristai+1)%3];
				memcpy(&aux->t->na[aristai],&aux->t->fn,3*sizeof(float));
				for (unsigned int m=0;m<objmesh.numtriangles;m++)
				{
					if (m!=i)
					{
						for (unsigned int aristam=0;aristam<3;aristam++)
						{
							unsigned int aristam0=objmesh.triangles[m].vindices[aristam];
							unsigned int aristam1=objmesh.triangles[m].vindices[(aristam+1)%3];
							if (((aristai0==aristam0)&&(aristai1==aristam1))||((aristai0==aristam1)&&(aristai1==aristam0)))
							{
								aux->t->na[aristai].x=aux->t->na[aristai].x+objmesh.facetnorms[objmesh.triangles[m].findex*3];
								aux->t->na[aristai].y=aux->t->na[aristai].y+objmesh.facetnorms[objmesh.triangles[m].findex*3+1];
								aux->t->na[aristai].z=aux->t->na[aristai].z+objmesh.facetnorms[objmesh.triangles[m].findex*3+2];
								// fuerzo a que termine de buscar otro triángulo con la misma arista 
								// pues pueden compartir una arista solamente dos triángulos diferentes
								m=objmesh.numtriangles;
								compartida=true;
								break;
							}
						}
					}
				}
				aux->t->na[aristai]=aux->t->na[aristai].Normalize();
				if (!compartida)
					printf("Arista %d del triángulo %d no compartida\n",aristai,i);
			}

			distaux=DistancePointTriangle(octree.root->pos,aux->t);
			if (distaux<mindist)
			{
				mindist=distaux;
				mintri=aux->t;
			}

			aux->next=octree.root->tri;
			octree.root->tri=aux;
			octree.root->numtriangles++;
		}
		else
		{
			delete aux->t;
			delete aux;
		}
	}
	
	if (!mintri)
	{
		octree.root->state=NODE_ALL_OUT;
	}
	else
	{
		if ((((mintri->v[1]-mintri->v[0])^(mintri->v[2]-mintri->v[0]))*(octree.root->pos-mintri->v[0]))>=0.0f)
			octree.root->state=NODE_PART_OUT;
		else
			octree.root->state=NODE_PART_IN;

		octree.root->distmin=mindist;
	}

	octree.Construct((unsigned int)(logf((float)VOXELSSIZE)/logf(2.0f))); //5 - 32

	ticks=GetTickCount();
	cout << ticks-ticksantes << " ms" << endl;
	
	#endif

	//////////////////////////
	// CREAMOS VOXELS
	//////////////////////////

	#ifdef BUDA

	cout << "Creating matrix " << VOXELSSIZE << "... ";

	ticksantes=GetTickCount();

	unsigned char* voxels=new unsigned char[VOXELSSIZE*VOXELSSIZE*VOXELSSIZE];
	memset(voxels,0,VOXELSSIZE*VOXELSSIZE*VOXELSSIZE);
	octree.CreateVoxels(VOXELSSIZE,voxels);
	ofstream file3d("test3d.raw",ios_base::binary);
	file3d.write((char*)voxels,VOXELSSIZE*VOXELSSIZE*VOXELSSIZE);
	file3d.close();
	delete[] voxels;

	ticks=GetTickCount();
	cout << ticks-ticksantes << " ms" << endl;
	
	#else

	unsigned int aux1,aux2;

	//FLBitmap2 bitmap2d1(256);
	//aux1=0;
	//for (unsigned int i=0;i<256;i++)
	//{
	//	for (unsigned int j=0;j<256;j++)
	//	{
	//		bitmap2d1.pixels[aux1+j]=sqrtf((float)((128-i)*(128-i)+(128-j)*(128-j)))<128.0f?255.0f:0.0f;
	//	}
	//	aux1+=256;
	//}
	//FLBitmap2 bitmap2d2(128);
	//bitmap2d1.SubSample2(bitmap2d2);
	//bitmap2d2.SubImage(bitmap2d1,0,0,128,128);
	//ofstream file2d("test2d.raw",ios_base::binary);
	//for (unsigned int i=0;i<bitmap2d1.numpixels;i++)
	//	file2d << (unsigned char)bitmap2d1.pixels[i];
	//file2d.close();

	FLBitmap3 bitmap3d1(VOXELSSIZE);
	aux1=0;
	#define RADIOESFERA 0.45f
	float distaux;
	for (unsigned int k=0;k<VOXELSSIZE;k++)
	{
		aux2=0;
		for (unsigned int i=0;i<VOXELSSIZE;i++)
		{			
			for (unsigned int j=0;j<VOXELSSIZE;j++)
			{
				float enk=(VOXELSSIZE-1)*0.5f-k;
				float enj=(VOXELSSIZE-1)*0.5f-j;
				float eni=(VOXELSSIZE-1)*0.5f-i;
				distaux=sqrtf(enk*enk+enj*enj+eni*eni);

				float radio=(VOXELSSIZE-1)*RADIOESFERA;
				if (distaux<(radio-1.0f))
				{
					distaux=255.0f;
				}
				else if (fabs(radio-distaux)<=1.0f)
				{
					distaux=(distaux-(radio-1.0f))*0.5f;
					distaux=255.0f-distaux*255.0f;
				}
				else
				{
					distaux=0.0f;
				}

				bitmap3d1.pixels[aux1+aux2+j]=distaux;
			}
			aux2+=VOXELSSIZE;
		}
		aux1+=VOXELSSIZE*VOXELSSIZE;
	}
	//FLBitmap3 bitmap3d2(VOXELSSIZE/2);
		//bitmap3d1.SubSample2(bitmap3d2);
	//memset(bitmap3d2.pixels,0,bitmap3d2.numpixels*sizeof(float));
		//bitmap3d1.Clear();
	//bitmap3d2.SubImage(bitmap3d1,0,0,0,VOXELSSIZE/2,VOXELSSIZE/2,VOXELSSIZE/2);
	ofstream file3d("test3d.raw",ios_base::binary);
	for (unsigned int i=0;i<bitmap3d1.numpixels;i++)
		file3d << (unsigned char)(bitmap3d1.pixels[i]+0.5f);
	file3d.close();

	#endif

	//////////////////////////
	// COMPRIMIMOS VOXELS
	//////////////////////////

	//system("FractalEncode.exe test3d.raw 32x32x32 1 2 const 2");

	//////////////////////////
	// DESCOMPRIMIMOS FRACTAL
	//////////////////////////

	/*FLFile file2("test3d.fra3","r+b");
	char clave[8];
	clave[0]=(char)file2.Unpack(8);
	clave[1]=(char)file2.Unpack(8);
	clave[2]=(char)file2.Unpack(8);
	clave[3]=(char)file2.Unpack(8);
	clave[4]=(char)file2.Unpack(8);
	clave[5]=(char)file2.Unpack(8);
	clave[6]=(char)file2.Unpack(8);
	clave[7]='\0';
	unsigned char version;
	version=(char)file2.Unpack(8);
	unsigned char type;
	type=(char)file2.Unpack(8);
	unsigned short tamxentrada=(unsigned short)file2.Unpack(16);
	unsigned short tamyentrada=(unsigned short)file2.Unpack(16);
	unsigned short tamzentrada=(unsigned short)file2.Unpack(16);
	unsigned char color=(file2.Unpack(1)==1);
	unsigned char tipofractal=(unsigned char)file2.Unpack(8);
	FLFractalConst3 fractal3;
	fractal3.imagetamx=tamxentrada;
	fractal3.imagetamy=tamyentrada;
	fractal3.imagetamz=tamzentrada;
	fractal3.Load(file2);
	FLBitmap3 inibit3(tamxentrada,tamyentrada,tamzentrada);
	for (unsigned int veces=0;veces<5;veces++)
		inibit3=fractal3.Decode(inibit3,0,1);
	ofstream salida("test.raw",std::ios_base::binary);
	unsigned char pixelcolor;
	for (unsigned int g=0;g<inibit3.numpixels;g++)
	{
		if (inibit3.pixels[g]>1.0f) 
			pixelcolor=255;
		else if (inibit3.pixels[g]<0.0f) 
			pixelcolor=0;
		else
			pixelcolor=(unsigned char)(inibit3.pixels[g]*255.0f+0.5f);
		salida << pixelcolor;
	}
	salida.close();*/

	//////////////////////////
	// VISUALIZAMOS VOXELS
	//////////////////////////

	//system("FractalView.exe test3d.fra3");
}
