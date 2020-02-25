#include "FLFractalQuad.h"
#include "FLDomains.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

FLFractalQuad2::FLFractalQuad2(void)
//:FLFractal2(FRACTAL_QUAD)
{
	//nummaxtransformations=nmt;
}

FLFractalQuad2::~FLFractalQuad2()
{
	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
	}
}

FLBitmap2 *bitmap2x2[4];

void FLFractalQuad2::DecodeRecurs(FLTransQuad2* trans,FLBitmap2 *salida,unsigned int zoom) const
{
	// truco: dónde queda 0
	// flip y rotate
	static int abcdef[8][6]={{1,0,0,1,0,0},{0,-1,1,0,1,0},{-1,0,0,-1,1,1},{0,1,-1,0,0,1},{-1,0,0,1,1,0},{0,-1,-1,0,1,1},{1,0,0,-1,0,1},{0,1,1,0,0,0}};

	if (trans->splitted)
	{
		DecodeRecurs(trans->child[0],salida,zoom);
		DecodeRecurs(trans->child[1],salida,zoom);
		DecodeRecurs(trans->child[2],salida,zoom);
		DecodeRecurs(trans->child[3],salida,zoom);
	}
	else
	{
		unsigned short i,j;
		unsigned short size=aplicazoom(trans->ransize,zoom);
		unsigned short x,y;

		if (trans->pis!=16)
		{
			int a,b,c,d,e,f;
			unsigned short posx,posy;	
			float *pixelpointer;

			a=abcdef[trans->ori][0];
			b=abcdef[trans->ori][1];
			c=abcdef[trans->ori][2];
			d=abcdef[trans->ori][3];
			e=aplicazoom(trans->ranposx,zoom)+abcdef[trans->ori][4]*(size-1);
			f=aplicazoom(trans->ranposy,zoom)+abcdef[trans->ori][5]*(size-1);	

			posx=aplicazoom(trans->domposx,zoom);
			posy=aplicazoom(trans->domposy,zoom);
			FLBitmap2* auxref2x2=bitmap2x2[((posy&1)<<1)|(posx&1)];				
			posx>>=1;
			posy>>=1;			
			
			for (j=0;j<size;j++)
			{
				x=b*j+e;
				y=d*j+f;
				pixelpointer=auxref2x2->pixels+(j+posy)*auxref2x2->tamx+posx;
				for (i=0;i<size;i++)
				{
					//if (x>=0 && x<salida->tamx && y>=0 && y<salida->tamy)
						salida->pixels[y*salida->tamx+x]=trans->s*(*pixelpointer)+trans->o; //level

					pixelpointer++;

					x+=a;
					y+=c;
				}
			}		
		}
		else
		{
			x=aplicazoom(trans->ranposx,zoom);
			y=aplicazoom(trans->ranposy,zoom);

			for (j=0;j<size;j++)
			{
				for (i=0;i<size;i++)
				{
					//if ((x+i)>=0 && (x+i)<salida->tamx && (y+j)>=0 && (y+j)<salida->tamy)
						salida->pixels[(y+j)*salida->tamx+(x+i)]=trans->o; //level
				}
			}
		}
	}
}

FLBitmap2 FLFractalQuad2::Decode(const FLBitmap2& bitmap,char zoom,unsigned int npasos) const
{
	FLBitmap2 salida(bitmap.tamx,bitmap.tamy);

	FLBitmap2 bitmap2x2aux(bitmap.tamx,bitmap.tamy);
	bitmap2x2[0]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
	bitmap.SubSample2(*bitmap2x2[0]);
	bitmap2x2[1]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
	bitmap.SubImage(bitmap2x2aux,1,0,bitmap.tamx-2,bitmap.tamy);
	bitmap2x2aux.SubSample2(*bitmap2x2[1]);
	bitmap2x2[2]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
	bitmap.SubImage(bitmap2x2aux,0,1,bitmap.tamx,bitmap.tamy-2);
	bitmap2x2aux.SubSample2(*bitmap2x2[2]);
	bitmap2x2[3]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
	bitmap.SubImage(bitmap2x2aux,1,1,bitmap.tamx-2,bitmap.tamy-2);
	bitmap2x2aux.SubSample2(*bitmap2x2[3]);

	for (unsigned int npaso=0;npaso<npasos;npaso++)
	{
 		for (unsigned int m=0;m<numtransformations;m++)
		{
			DecodeRecurs((FLTransQuad2*)transformations[m],&salida,zoom);
		}
	}
	
	delete bitmap2x2[0];
	delete bitmap2x2[1];
	delete bitmap2x2[2];
	delete bitmap2x2[3];

	return salida;
}

//float FLFractalQuad2::Decode(const FLVector2& pos,unsigned int deep) const
float FLFractalQuad2::Decode(const FLVector2& pos,float prec,float sem,unsigned int& deep) const
{
	FLVector2 posaux=pos;
	deep=1;
	float colorultimo=sem;
	float colorpenultimo;
   
	float factor0=1.0f;
	float factor1=0.0f;
    float factoracum=0.0f;
	
	FLTransQuad2* trans;
	unsigned short ransize;
	unsigned short ranx,rany;
	unsigned char son;

	/*static unsigned int cont=0;
	std::ofstream file;
	file.open("test.txt",std::ios_base::app);
	file << colorultimo << std::endl;*/

	//truco: igual que el otro decode pero sin matrices invertidas (si desplazamientos invertidos)
	static float abcdef[8][6]={{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f},{0.0f,1.0f,-1.0f,0.0f,1.0f,0.0f},{-1.0f,0.0f,0.0f,-1.0f,1.0f,1.0f},{0.0f,-1.0f,1.0f,0.0f,0.0f,1.0f},{-1.0f,0.0f,0.0f,1.0f,1.0f,0.0f},{0.0f,-1.0f,-1.0f,0.0f,1.0f,1.0f},{1.0f,0.0f,0.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f,1.0f,0.0f,0.0f,0.0f}};
	float a,b,c,d,e,f;
	float relx,rely;

	do
	{
		if (posaux.x<0.0f) posaux.x=0.0f;
		if (posaux.y<0.0f) posaux.y=0.0f;
		if (posaux.x>1.0f) posaux.x=1.0f;
		if (posaux.y>1.0f) posaux.y=1.0f;

		// calculo punto a transformar
		float posxf=posaux.x*(float)(imagetamx);
		float posyf=posaux.y*(float)(imagetamy);
		unsigned short posx=(unsigned short)(posxf*0.99999f);
		unsigned short posy=(unsigned short)(posyf*0.99999f);
		//float posxf=(float)posx;
		//float posyf=(float)posy;

		// busco en que trasformación caeria posx,posy
		// m será la transformación base dentro de numbasetrans
		ransize=16;
		ranx=(posx/ransize)*ransize;
		rany=(posy/ransize)*ransize;
		trans=(FLTransQuad2*)transformations[(posy/ransize)*(imagetamx/ransize)+(posx/ransize)];
		while (trans->splitted)
		{
			ransize>>=1;
			son=(((posy-rany)>=ransize)<<1)|((posx-ranx)>=ransize);
			if (son&1) ranx+=ransize;
			if (son&2) rany+=ransize;
			trans=trans->child[son];			
		}        

		// aplico transformación
		if (trans->pis!=16)
		{
			a=abcdef[trans->ori][0];
			b=abcdef[trans->ori][1];
			c=abcdef[trans->ori][2];
			d=abcdef[trans->ori][3];
			e=(float)trans->ransize*abcdef[trans->ori][4];
			f=(float)trans->ransize*abcdef[trans->ori][5];

			relx=posxf-e-(float)trans->ranposx;
			rely=posyf-f-(float)trans->ranposy;

			posaux.x=a*relx+b*rely;
			posaux.y=c*relx+d*rely;

			posaux.x*=2.0f;
			posaux.y*=2.0f;

			posaux.x+=(float)trans->domposx;
			posaux.y+=(float)trans->domposy;
			
			posaux.x/=(float)(imagetamx);
			posaux.y/=(float)(imagetamy);

			colorpenultimo=colorultimo;
			
			factoracum+=factor1;
			factor1=factor0*trans->o;
			factor0*=trans->s;
			
			colorultimo=sem*factor0+factor1+factoracum;
		}
		else
		{
			factoracum+=factor1;
			factor1=factor0*trans->o;

			colorultimo=factor1+factoracum;

			//file << colorultimo << std::endl;

			break;
		}	
		
		/*if (colorultimo<0.0f)
			colorultimo=0.0f;
		else if (colorultimo>255.0f)
			colorultimo=255.0f;*/

		deep++;

		//file << colorultimo << std::endl;
	}
	while(deep<5 || (deep<51 && fabs(colorpenultimo-colorultimo)>prec));

	/*file << std::endl;
	file << std::endl;
	//file << deep << std::endl;
	file.close();
	cont++;*/

	return colorultimo;
}

void FLFractalQuad2::DrawBoundsRecurs(FLTransQuad2 *trans,FLBitmap2 *bitmap,char zoom) const
{
	if (trans->splitted)
	{
		DrawBoundsRecurs(trans->child[0],bitmap,zoom);
		DrawBoundsRecurs(trans->child[1],bitmap,zoom);
		DrawBoundsRecurs(trans->child[2],bitmap,zoom);
		DrawBoundsRecurs(trans->child[3],bitmap,zoom);
	}
	else
	{
		unsigned int size=aplicazoom(trans->ransize,zoom);

		unsigned int x,y;
		for (unsigned int j=0;j<size;j++)
		{
			for (unsigned int i=0;i<size;i+=(j!=0)?size:1)
			{
				x=i+aplicazoom(trans->ranposx,zoom);
				y=j+aplicazoom(trans->ranposy,zoom);
                if (x>=0 && x<bitmap->tamx && y>=0 && y<bitmap->tamy)
					bitmap->pixels[y*bitmap->tamx+x]=1.0f;
			}
		}
	}
}

void FLFractalQuad2::DrawBounds(FLBitmap2& bitmap,char zoom) const
{
	for (unsigned int m=0;m<numtransformations;m++)
	{
		DrawBoundsRecurs((FLTransQuad2*)transformations[m],&bitmap,zoom);
	}
}

void FLFractalQuad2::SmoothBoundsRecurs(FLTransQuad2 *trans,FLBitmap2 *bitmap,FLBitmap2 *salida,char zoom) const
{
	if (trans->splitted)
	{
		SmoothBoundsRecurs(trans->child[0],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[1],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[2],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[3],bitmap,salida,zoom);
	}
	else
	{
		unsigned short rx,ry;
		rx=aplicazoom(trans->ranposx,zoom);
		ry=aplicazoom(trans->ranposy,zoom);

		unsigned int size=aplicazoom(trans->ransize,zoom);
		float w1,w2;
		float pixel1,pixel2;

		if (trans->ransize==4)
		{
			w1=5.0f;
			w2=1.0f;
		}
		else
		{
			w1=2.0f;
			w2=1.0f;
		}	

		if (ry!=0)
		{
			for (unsigned int i=0;i<size;++i) 
			{
				pixel1=bitmap->pixels[ry*bitmap->tamx+rx+i];
				pixel2=bitmap->pixels[(ry-1)*bitmap->tamx+rx+i];
				salida->pixels[ry*bitmap->tamx+rx+i]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida->pixels[(ry-1)*bitmap->tamx+rx+i]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}

		if (rx!=0)
		{
			for (unsigned int j=0;j<size;++j) 
			{
				pixel1=bitmap->pixels[(ry+j)*bitmap->tamx+rx];
				pixel2=bitmap->pixels[(ry+j)*bitmap->tamx+rx-1];
				salida->pixels[(ry+j)*bitmap->tamx+rx]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida->pixels[(ry+j)*bitmap->tamx+rx-1]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}
	}
}

void FLFractalQuad2::SmoothBounds(FLBitmap2& bitmap,char zoom) const
{
	FLBitmap2 salida(bitmap.tamx,bitmap.tamy);
	salida=bitmap;

	for (unsigned int m=0;m<numtransformations;m++)
		SmoothBoundsRecurs((FLTransQuad2*)transformations[m],&bitmap,&salida,zoom);

	bitmap=salida;
}

void FLFractalQuad2::Encode(FLBitmap2 *bitmap,unsigned short domainsalto,unsigned int nummaxtransformations)
{
	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	imagetamx=bitmap->tamx;
	imagetamy=bitmap->tamy;

	// precalculamos los posibles dominios
	unsigned int domaintam=32;
	unsigned int firstrange=domaintam>>1;
	unsigned int lastrange=2;
	unsigned int numlevels=4;
	
	FLDomains2** domains=new FLDomains2*[numlevels]; //32,16,8,4,2?
	
	domains[0]=new FLDomains2(bitmap,domaintam,domainsalto);
	domaintam=domaintam>>1;

	// aproximamos los rangos
	numbasetrans=(imagetamy/firstrange)*(imagetamx/firstrange);
	numtransformations=0;
	cout << "solving transformations [" << nummaxtransformations << "] = 0";
	cout.flush();
	vector<FLTrans2*> vtransformations;
	transformations=new FLTrans2*[numbasetrans];

	// creo primeros niveles hasta rangos de firstrange en orden
	FLTrans2* transaux;
	for (unsigned int j=0;j<imagetamy;j+=firstrange)
	{
		for (unsigned int i=0;i<imagetamx;i+=firstrange)
		{
			FLBitmapRef2 range(i,j,i,j,firstrange,firstrange,bitmap);
			//range.SolveSums();

			transaux=new FLTransQuad2();
			domains[0]->OptimizeTrans(transaux,range);
            
			vector<FLTrans2*>::iterator it=vtransformations.begin();
			while (it!=vtransformations.end())
			{
				if ((*it)->rms<transaux->rms)
					break;
				it++;
			}
			vtransformations.insert(it,transaux);

			transformations[numtransformations]=(FLTrans2*)transaux;	
			numtransformations++;

			if ((numtransformations%100)==0)
			{
				cout << "," << numtransformations;
				cout.flush();
			}
		}		
	}

	cout << endl;

	delete domains[0];

	for (unsigned int h2=1;h2<numlevels;h2++)
	{
		domains[h2]=new FLDomains2(bitmap,domaintam,domainsalto);
		domaintam=domaintam>>1;
	}

	cout << "solving transformations [" << nummaxtransformations << "] = ";
	cout.flush();
	
	while(numtransformations<nummaxtransformations)
	{
		// cojo el peor
		vector<FLTrans2*>::iterator it2=vtransformations.begin();
		while (it2!=vtransformations.end())
		{
			if ((*it2)->ransize>lastrange)
				break;
			it2++;
		}
		if (it2==vtransformations.end())
		{
			cout << "all==" << lastrange;
			cout.flush();
			break;
		}
		FLTransQuad2* worsttrans=(FLTransQuad2*)(*it2);
		// lo saco del vector
		vtransformations.erase(it2);
		
		worsttrans->splitted=true;
		unsigned int numchild=0;

		unsigned int qsize=worsttrans->ransize>>1;
		unsigned char qdomain=numlevels-1;
		while(qsize>lastrange) 
		{
			qsize=qsize>>1;
			qdomain--;
		}

		// creo sus hijos y los añado ordenados
		for (unsigned short j2=worsttrans->ranposy;j2<worsttrans->ranposy+worsttrans->ransize;j2+=worsttrans->ransize>>1)
		{
			for (unsigned short i2=worsttrans->ranposx;i2<worsttrans->ranposx+worsttrans->ransize;i2+=worsttrans->ransize>>1)
			{
				FLTransQuad2* transaux=new FLTransQuad2();
					
				/*if ((worsttrans->ransize>>1)==1)
				{
					FLBitmap2 rangeraw(2,2);
					bitmap->SuperSample2(i2,j2,rangeraw);
					FLBitmapRef2 range(i2,j2,0,0,2,2,&rangeraw);								
					domains[qdomain]->OptimizeTrans(transaux,range);
				}
				else*/
				{
					FLBitmapRef2 range(i2,j2,i2,j2,worsttrans->ransize>>1,worsttrans->ransize>>1,bitmap);
					domains[qdomain]->OptimizeTrans(transaux,range);
				}

				//if (transaux->rms>worsttrans->rms)
				//	transaux->rms=worsttrans->rms*0.5f;
					        
				vector<FLTrans2*>::iterator it3=vtransformations.begin();
				while (it3!=vtransformations.end())
				{
					if ((*it3)->rms<transaux->rms)
						break;
					it3++;
				}
				vtransformations.insert(it3,transaux);

				worsttrans->child[numchild]=transaux;
				numchild++;
			}
		}
		// libero memoria
		//delete worsttrans;
		// aumento contador
		numtransformations+=3;

		if (abs((int)numtransformations%100)<3)
		{
			cout << "," << numtransformations;
			cout.flush();
		}
	}
	if (abs((int)numtransformations%100)>=3)
		cout << "," << numtransformations;

	cout << endl;

	// vuelco transformaciones
	//transformations=new FLTrans2*[numtransformations];
	/*for (vector<FLTrans2*>::iterator it=vtransformations.begin();it!=vtransformations.end();it++)
	{
		transformations[actualtransformation]=(FLTrans2*)(*it);	
		actualtransformation++;
	}*/

	vtransformations.clear();
	
	for (unsigned int h2=1;h2<numlevels;h2++)
	{
		delete domains[h2];
	}
	delete domains;
}
void FLFractalQuad2::Save(FLFile& file) const
{
	// peor caso: 1 bit + numtransformations*48 bits 
	// mejor caso: 1 bit + numtransformations*1 bits 
	
	// type 1 bit
	// numtransformations transformaciones

	//file.Pack(8,type);
	for (unsigned int i=0;i<numbasetrans;i++)
	{
		FLTransQuad2* trans=(FLTransQuad2*)transformations[i];
		trans->Save(file);
	}
}

void FLFractalQuad2::Load(FLFile& file)
{
	// peor caso: numtransformations*48 bits 
	// mejor caso: numtransformations*1 bits 
	
	// numtransformations transformaciones

	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	numbasetrans=(imagetamx/16)*(imagetamy/16);
	//numtransformations=(unsigned short)file.Unpack(16);
    transformations=new FLTrans2*[numbasetrans];
	numtransformations=0;

	for (unsigned short j=0;j<imagetamy;j+=16)
	{
		for (unsigned short i=0;i<imagetamx;i+=16)
		{
			transformations[numtransformations]=new FLTransQuad2();
			((FLTransQuad2*)transformations[numtransformations])->Load(file,i,j,16);
			/*basetrans[numtrans].domsize=basetrans[numtrans].ransize<<1;
			basetrans[numtrans].s=(float)basetrans[numtrans].pis/(float)(1<<5)*2.0f-1.0f;
			basetrans[numtrans].o=(float)basetrans[numtrans].pio/(float)((1<<7)-1)*((1.0f+fabs(basetrans[numtrans].s))*255.0f);
			if (basetrans[numtrans].s>0.0) basetrans[numtrans].o-=basetrans[numtrans].s*255.0f;*/
			numtransformations++;
		}
	}
}

FLFractalQuad3::FLFractalQuad3(void)
//:FLFractal2(FRACTAL_QUAD)
{
	//nummaxtransformations=nmt;
}

FLFractalQuad3::~FLFractalQuad3()
{
	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
	}
}

FLBitmap3 *bitmap3_2x2[8];

void FLFractalQuad3::DecodeRecurs(FLTransQuad3* trans,FLBitmap3 *salida,unsigned int zoom) const
{
	if (trans->splitted)
	{
		DecodeRecurs(trans->child[0],salida,zoom);
		DecodeRecurs(trans->child[1],salida,zoom);
		DecodeRecurs(trans->child[2],salida,zoom);
		DecodeRecurs(trans->child[3],salida,zoom);
		DecodeRecurs(trans->child[4],salida,zoom);
		DecodeRecurs(trans->child[5],salida,zoom);
		DecodeRecurs(trans->child[6],salida,zoom);
		DecodeRecurs(trans->child[7],salida,zoom);
	}
	else
	{
		unsigned short i,j,k;
		unsigned short size=aplicazoom(trans->ransize,zoom);

		if (trans->pis!=16)
		{
			int m[9];
			int dx,dy,dz;
			int aux1=size-1;
			dx=aplicazoom(trans->ranposx,zoom);
			dy=aplicazoom(trans->ranposy,zoom);
			dz=aplicazoom(trans->ranposz,zoom);

			switch(trans->ori)
			{
				#define matrix(m0,m1,m2,m3,m4,m5,m6,m7,m8) m[0]=m0;m[1]=m1;m[2]=m2;m[3]=m3;m[4]=m4;m[5]=m5;m[6]=m6;m[7]=m7;m[8]=m8;

				case 0:
					matrix(1,0,0,0,1,0,0,0,1)
					break;
				case 1:
					matrix(1,0,0,0,0,-1,0,1,0)
					dy+=aux1;
					break;
				case 2:
					matrix(0,0,1,0,1,0,-1,0,0)
					dz+=aux1;
					break;
				case 3:
					matrix(0,-1,0,1,0,0,0,0,1)
					dx+=aux1;
					break;
				case 4:
					matrix(-1,0,0,0,1,0,0,0,1)
					dx+=aux1;
					break;
				case 5:
					matrix(-1,0,0,0,0,-1,0,1,0)
					dx+=aux1;
					dy+=aux1;
					break;
				case 6:
					matrix(0,0,1,0,1,0,1,0,0)
					break;
				case 7:
					matrix(0,-1,0,-1,0,0,0,0,1)
					dx+=aux1;
					dy+=aux1;
					break;
				case 8:
					matrix(1,0,0,0,-1,0,0,0,1)
					dy+=aux1;
					break;
				case 9:
					matrix(1,0,0,0,0,-1,0,-1,0)
					dy+=aux1;
					dz+=aux1;
					break;
				case 10:
					matrix(0,0,1,0,-1,0,-1,0,0)
					dy+=aux1;
					dz+=aux1;
					break;
				case 11:
					matrix(0,1,0,1,0,0,0,0,1)
					break;
				case 12:
					matrix(1,0,0,0,1,0,0,0,-1)
					dz+=aux1;
					break;
				case 13:
					matrix(1,0,0,0,0,1,0,1,0)
					break;
				case 14:
					matrix(0,0,-1,0,1,0,-1,0,0)
					dx+=aux1;
					dz+=aux1;
					break;
				case 15:
					matrix(0,-1,0,1,0,0,0,0,-1)
					dx+=aux1;
					dz+=aux1;
					break;
				case 16:
					matrix(-1,0,0,0,-1,0,0,0,1)
					dx+=aux1;
					dy+=aux1;
					break;
				case 17:
					matrix(-1,0,0,0,0,-1,0,-1,0)
					dx+=aux1;
					dy+=aux1;
					dz+=aux1;
					break;
				case 18:
					matrix(0,0,1,0,-1,0,1,0,0)
					dy+=aux1;
					break;
				case 19:
					matrix(0,1,0,-1,0,0,0,0,1)
					dy+=aux1;
					break;
				case 20:
					matrix(-1,0,0,0,1,0,0,0,-1)
					dx+=aux1;
					dz+=aux1;
					break;
				case 21:
					matrix(-1,0,0,0,0,1,0,1,0)
					dx+=aux1;
					break;
				case 22:
					matrix(0,0,-1,0,1,0,1,0,0)
					dx+=aux1;
					break;
				case 23:
					matrix(0,-1,0,-1,0,0,0,0,-1)
					dx+=aux1;
					dy+=aux1;
					dz+=aux1;
					break;
				case 24:
					matrix(1,0,0,0,-1,0,0,0,-1)
					dy+=aux1;
					dz+=aux1;
					break;
				case 25:
					matrix(1,0,0,0,0,1,0,-1,0)
					dz+=aux1;
					break;
				case 26:
					matrix(0,0,-1,0,-1,0,-1,0,0)
					dx+=aux1;
					dy+=aux1;
					dz+=aux1;
					break;
				case 27:
					matrix(0,1,0,1,0,0,0,0,-1)
					dz+=aux1;
					break;
				case 28:
					matrix(-1,0,0,0,-1,0,0,0,-1)
					dx+=aux1;
					dy+=aux1;
					dz+=aux1;
					break;
				case 29:
					matrix(-1,0,0,0,0,1,0,-1,0)
					dx+=aux1;
					dz+=aux1;
					break;
				case 30:
					matrix(0,0,-1,0,-1,0,1,0,0)
					dx+=aux1;
					dy+=aux1;
					break;
				case 31:
					matrix(0,1,0,-1,0,0,0,0,-1)
					dy+=aux1;
					dz+=aux1;
					break;
				default:
					matrix(1,0,0,0,1,0,0,0,1)
					break;
			}
			
			unsigned short posx=aplicazoom(trans->domposx,zoom);
			unsigned short posy=aplicazoom(trans->domposy,zoom);
			unsigned short posz=aplicazoom(trans->domposz,zoom);
			FLBitmap3* auxref2x2=bitmap3_2x2[((posz&1)<<2)|((posy&1)<<1)|(posx&1)];
			posx>>=1;
			posy>>=1;
			posz>>=1;

			unsigned short x,y,z;
			float *pixelpointer;
			for (k=0;k<size;k++)
			{
				for (j=0;j<size;j++)
				{
					x=m[2]*k+m[1]*j+dx;
					y=m[5]*k+m[4]*j+dy;
					z=m[8]*k+m[7]*j+dz;

					pixelpointer=auxref2x2->pixels+(k+posz)*auxref2x2->tamxpertamy+(j+posy)*auxref2x2->tamx+posx;
					for (i=0;i<size;i++)
					{
						/*if ((z*salida.tamy*salida.tamx+y*salida.tamx+x)>(32*32*32))
							int sdfasd=3;*/

						//if (x>=0 && x<salida->tamx && y>=0 && y<salida->tamy)
							salida->pixels[z*salida->tamxpertamy+y*salida->tamx+x]=trans->s*(*pixelpointer)+trans->o; //level

						pixelpointer++;

						x+=m[0];
						y+=m[3];
						z+=m[6];
					}
				}
			}
		}
		else
		{
			unsigned short x=aplicazoom(trans->ranposx,zoom);
			unsigned short y=aplicazoom(trans->ranposy,zoom);
			unsigned short z=aplicazoom(trans->ranposz,zoom);

			for (k=0;k<size;k++)
			{
				for (j=0;j<size;j++)
				{
					for (i=0;i<size;i++)
					{
						//if ((x+i)>=0 && (x+i)<salida->tamx && (y+j)>=0 && (y+j)<salida->tamy)
							salida->pixels[(z+k)*salida->tamxpertamy+(y+j)*salida->tamx+(x+i)]=trans->o; //level
					}
				}
			}
		}
	}
}

FLBitmap3 FLFractalQuad3::Decode(const FLBitmap3& bitmap,char zoom,unsigned int npasos) const
{
	FLBitmap3 salida(bitmap.tamx,bitmap.tamy,bitmap.tamz);

	FLBitmap3 bitmap2x2aux(bitmap.tamx,bitmap.tamy,bitmap.tamz);
	bitmap3_2x2[0]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubSample2(*bitmap3_2x2[0]);
	bitmap3_2x2[1]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,1,0,0,(bitmap.tamx)-2,(bitmap.tamy),(bitmap.tamz));
	bitmap2x2aux.SubSample2(*bitmap3_2x2[1]);
	bitmap3_2x2[2]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,0,1,0,(bitmap.tamx),(bitmap.tamy)-2,(bitmap.tamz));
	bitmap2x2aux.SubSample2(*bitmap3_2x2[2]);
	bitmap3_2x2[3]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,1,1,0,(bitmap.tamx)-2,(bitmap.tamy)-2,(bitmap.tamz));
	bitmap2x2aux.SubSample2(*bitmap3_2x2[3]);
	bitmap3_2x2[4]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,0,0,1,(bitmap.tamx),(bitmap.tamy),(bitmap.tamz)-2);
	bitmap2x2aux.SubSample2(*bitmap3_2x2[4]);
	bitmap3_2x2[5]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,1,0,1,(bitmap.tamx)-2,(bitmap.tamy),(bitmap.tamz)-2);
	bitmap2x2aux.SubSample2(*bitmap3_2x2[5]);
	bitmap3_2x2[6]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,0,1,1,(bitmap.tamx),(bitmap.tamy)-2,(bitmap.tamz)-2);
	bitmap2x2aux.SubSample2(*bitmap3_2x2[6]);
	bitmap3_2x2[7]=new FLBitmap3((bitmap.tamx)>>1,(bitmap.tamy)>>1,(bitmap.tamz)>>1);
	bitmap.SubImage(bitmap2x2aux,1,1,1,(bitmap.tamx)-2,(bitmap.tamy)-2,(bitmap.tamz)-2);
	bitmap2x2aux.SubSample2(*bitmap3_2x2[7]);

	for (unsigned int npaso=0;npaso<npasos;npaso++)
	{
 		for (unsigned int m=0;m<numtransformations;m++)
		{
			DecodeRecurs((FLTransQuad3*)transformations[m],&salida,zoom);
		}
	}
	
	delete bitmap3_2x2[0];
	delete bitmap3_2x2[1];
	delete bitmap3_2x2[2];
	delete bitmap3_2x2[3];
	delete bitmap3_2x2[4];
	delete bitmap3_2x2[5];
	delete bitmap3_2x2[6];
	delete bitmap3_2x2[7];

	return salida;
}

//float FLFractalQuad2::Decode(const FLVector2& pos,unsigned int deep) const
float FLFractalQuad3::Decode(const FLVector3& pos,float prec,float sem,unsigned int& deep) const
{
	FLVector3 posaux=pos;
	deep=1;
	float colorultimo=sem;
	float colorpenultimo;
   
	float factor0=1.0f;
    float factoracum=0.0f;
	float factorultimo=0.0f;
	
	FLTransQuad3* transaux;
	unsigned short ransize;
	unsigned short ranx,rany,ranz;
	unsigned char son;

	/*static unsigned int cont=0;
	std::ofstream file;
	file.open("test.txt",std::ios_base::app);
	file << colorultimo << std::endl;*/

	do
	{
		if (posaux.x<0.0f) posaux.x=0.0f;
		if (posaux.y<0.0f) posaux.y=0.0f;
		if (posaux.z<0.0f) posaux.z=0.0f;
		if (posaux.x>1.0f) posaux.x=1.0f;
		if (posaux.y>1.0f) posaux.y=1.0f;
		if (posaux.z>1.0f) posaux.z=1.0f;

		// calculo punto a transformar
		float posxf=posaux.x*(float)(imagetamx);
		float posyf=posaux.y*(float)(imagetamy);
		float poszf=posaux.z*(float)(imagetamz);
		unsigned short posx=(unsigned short)(posxf*0.99999f);
		unsigned short posy=(unsigned short)(posyf*0.99999f);
		unsigned short posz=(unsigned short)(poszf*0.99999f);
		//float posxf=(float)posx;
		//float posyf=(float)posy;

		// busco en que trasformación caeria posx,posy,posz
		// m será la transformación base dentro de numbasetrans
		ransize=16;
		ranx=(posx/ransize)*ransize;
		rany=(posy/ransize)*ransize;
		ranz=(posz/ransize)*ransize;
		transaux=(FLTransQuad3*)transformations[(posz/ransize)*(imagetamy/ransize)*(imagetamx/ransize)+(posy/ransize)*(imagetamx/ransize)+(posx/ransize)];
		while (transaux->splitted)
		{
			ransize>>=1;
			son=(((posz-ranz)>=ransize)<<2)|(((posy-rany)>=ransize)<<1)|((posx-ranx)>=ransize);
			if (son&1) ranx+=ransize;
			if (son&2) rany+=ransize;
			if (son&4) ranz+=ransize;
			transaux=transaux->child[son];			
		}        

		// aplico transformación
		if (transaux->pis!=16)
		{
			float m[9];
			float dx,dy,dz;
			float aux1=(float)transaux->ransize;
            switch(transaux->ori)
			{
				#define matrixf(m0,m1,m2,m3,m4,m5,m6,m7,m8) m[0]=m0;m[1]=m1;m[2]=m2;m[3]=m3;m[4]=m4;m[5]=m5;m[6]=m6;m[7]=m7;m[8]=m8;

				case 0:
					matrixf(1,0,0,0,1,0,0,0,1)
					dx=0.0f;
					dy=0.0f;
					dz=0.0f;
					break;
				case 1:
					matrixf(1,0,0,0,0,-1,0,1,0)
					dx=0.0f;
					dy=aux1;
					dz=0.0f;
					break;
				case 2:
					matrixf(0,0,1,0,1,0,-1,0,0)
					dx=0.0f;
					dy=0.0f;
					dz=aux1;
					break;
				case 3:
					matrixf(0,-1,0,1,0,0,0,0,1)
					dx=aux1;
					dy=0.0f;
					dz=0.0f;
					break;
				case 4:
					matrixf(-1,0,0,0,1,0,0,0,1)
					dx=aux1;
					dy=0.0f;
					dz=0.0f;
					break;
				case 5:
					matrixf(-1,0,0,0,0,-1,0,1,0)
					dx=aux1;
					dy=aux1;
					dz=0.0f;
					break;
				case 6:
					matrixf(0,0,1,0,1,0,1,0,0)
					dx=0.0f;
					dy=0.0f;
					dz=0.0f;
					break;
				case 7:
					matrixf(0,-1,0,-1,0,0,0,0,1)
					dx=aux1;
					dy=aux1;
					dz=0.0f;
					break;
				case 8:
					matrixf(1,0,0,0,-1,0,0,0,1)
					dx=0.0f;
					dy=aux1;
					dz=0.0f;
					break;
				case 9:
					matrixf(1,0,0,0,0,-1,0,-1,0)
					dx=0.0f;
					dy=aux1;
					dz=aux1;
					break;
				case 10:
					matrixf(0,0,1,0,-1,0,-1,0,0)
					dx=0.0f;
					dy=aux1;
					dz=aux1;
					break;
				case 11:
					matrixf(0,1,0,1,0,0,0,0,1)
					dx=0.0f;
					dy=0.0f;
					dz=0.0f;
					break;
				case 12:
					matrixf(1,0,0,0,1,0,0,0,-1)
					dx=0.0f;
					dy=0.0f;
					dz=aux1;
					break;
				case 13:
					matrixf(1,0,0,0,0,1,0,1,0)
					dx=0.0f;
					dy=0.0f;
					dz=0.0f;
					break;
				case 14:
					matrixf(0,0,-1,0,1,0,-1,0,0)
					dx=aux1;
					dy=0.0f;
					dz=aux1;
					break;
				case 15:
					matrixf(0,-1,0,1,0,0,0,0,-1)
					dx=aux1;
					dy=0.0f;
					dz=aux1;
					break;
				case 16:
					matrixf(-1,0,0,0,-1,0,0,0,1)
					dx=aux1;
					dy=aux1;
					dz=0.0f;
					break;
				case 17:
					matrixf(-1,0,0,0,0,-1,0,-1,0)
					dx=aux1;
					dy=aux1;
					dz=aux1;
					break;
				case 18:
					matrixf(0,0,1,0,-1,0,1,0,0)
					dx=0.0f;
					dy=aux1;
					dz=0.0f;
					break;
				case 19:
					matrixf(0,1,0,-1,0,0,0,0,1)
					dx=0.0f;
					dy=aux1;
					dz=0.0f;
					break;
				case 20:
					matrixf(-1,0,0,0,1,0,0,0,-1)
					dx=aux1;
					dy=0.0f;
					dz=aux1;
					break;
				case 21:
					matrixf(-1,0,0,0,0,1,0,1,0)
					dx=aux1;
					dy=0.0f;
					dz=0.0f;
					break;
				case 22:
					matrixf(0,0,-1,0,1,0,1,0,0)
					dx=aux1;
					dy=0.0f;
					dz=0.0f;
					break;
				case 23:
					matrixf(0,-1,0,-1,0,0,0,0,-1)
					dx=aux1;
					dy=aux1;
					dz=aux1;
					break;
				case 24:
					matrixf(1,0,0,0,-1,0,0,0,-1)
					dx=0.0f;
					dy=aux1;
					dz=aux1;
					break;
				case 25:
					matrixf(1,0,0,0,0,1,0,-1,0)
					dx=0.0f;
					dy=0.0f;
					dz=aux1;
					break;
				case 26:
					matrixf(0,0,-1,0,-1,0,-1,0,0)
					dx=aux1;
					dy=aux1;
					dz=aux1;
					break;
				case 27:
					matrixf(0,1,0,1,0,0,0,0,-1)
					dx=0.0f;
					dy=0.0f;
					dz=aux1;
					break;
				case 28:
					matrixf(-1,0,0,0,-1,0,0,0,-1)
					dx=aux1;
					dy=aux1;
					dz=aux1;
					break;
				case 29:
					matrixf(-1,0,0,0,0,1,0,-1,0)
					dx=aux1;
					dy=0.0f;
					dz=aux1;
					break;
				case 30:
					matrixf(0,0,-1,0,-1,0,1,0,0)
					dx=aux1;
					dy=aux1;
					dz=0.0f;
					break;
				case 31:
					matrixf(0,1,0,-1,0,0,0,0,-1)
					dx=0.0f;
					dy=aux1;
					dz=aux1;
					break;
				default:
					matrixf(1,0,0,0,1,0,0,0,1)
					dx=0.0f;
					dy=0.0f;
					dz=0.0f;
					break;
			}


			float relx=posxf-dx-(float)transaux->ranposx;
			float rely=posyf-dy-(float)transaux->ranposy;
			float relz=poszf-dz-(float)transaux->ranposz;

			posaux.x=m[0]*relx+m[3]*rely+m[6]*relz;
			posaux.y=m[1]*relx+m[4]*rely+m[7]*relz;
			posaux.z=m[2]*relx+m[5]*rely+m[8]*relz;

			posaux.x*=2.0f;
			posaux.y*=2.0f;
			posaux.z*=2.0f;

			posaux.x+=(float)transaux->domposx;
			posaux.y+=(float)transaux->domposy;
			posaux.z+=(float)transaux->domposz;
			
			posaux.x/=(float)(imagetamx);
			posaux.y/=(float)(imagetamy);
			posaux.z/=(float)(imagetamz);

			colorpenultimo=colorultimo;
			
			factoracum+=factorultimo;
			factorultimo=factor0*transaux->o;
			factor0*=transaux->s;
			
			colorultimo=sem*factor0+factoracum+factorultimo;
		}
		else
		{
			factoracum+=factorultimo;
			factorultimo=factor0*transaux->o;
			colorultimo=factoracum+factorultimo;

			return colorultimo;
			
			//break; //?
		}	
		
		/*if (colorultimo<0.0f)
			colorultimo=0.0f;
		else if (colorultimo>255.0f)
			colorultimo=255.0f;*/

		deep++;

		//file << colorultimo << std::endl;
	}
	while(deep<4 || (deep<51 && fabs(colorpenultimo-colorultimo)>prec));

	/*file << std::endl;
	file << std::endl;
	//file << deep << std::endl;
	file.close();
	cont++;*/

	return colorultimo;
}

void FLFractalQuad3::DrawBoundsRecurs(FLTransQuad3 *trans,FLBitmap3 *bitmap,char zoom) const
{
	if (trans->splitted)
	{
		DrawBoundsRecurs(trans->child[0],bitmap,zoom);
		DrawBoundsRecurs(trans->child[1],bitmap,zoom);
		DrawBoundsRecurs(trans->child[2],bitmap,zoom);
		DrawBoundsRecurs(trans->child[3],bitmap,zoom);
		DrawBoundsRecurs(trans->child[4],bitmap,zoom);
		DrawBoundsRecurs(trans->child[5],bitmap,zoom);
		DrawBoundsRecurs(trans->child[6],bitmap,zoom);
		DrawBoundsRecurs(trans->child[7],bitmap,zoom);
	}
	else
	{
		/*unsigned int size=trans->ransize<<zoom;

		float *pointer;
		unsigned int x,y,z;
		z=trans->ranposz<<zoom;
		for (unsigned int k=0;k<size;k++)
		{
			y=trans->ranposy<<zoom;
			for (unsigned int j=0;j<size;j++)
			{
				for (unsigned int i=0;i<size;i+=((k==0)|(j==0))?1:size)
				{
					x=i+(trans->ranposx<<zoom);
					bitmap->pixels[z*bitmap->tamx*bitmap->tamy+y*bitmap->tamx+x]=1.0f;
				}
				y++;
			}
			z++;
		}*/

		unsigned int size=aplicazoom(trans->ransize,zoom);

		unsigned int aux1=bitmap->tamxpertamy-size*bitmap->tamx;

		float *pointer=bitmap->pixels+aplicazoom(trans->ranposx,zoom)+aplicazoom(trans->ranposy,zoom)*bitmap->tamx+aplicazoom(trans->ranposz,zoom)*bitmap->tamxpertamy;
		unsigned int salto;
		for (unsigned int k=0;k<size;k++)
		{
			salto=1;
			for (unsigned int j=0;j<size;j++)
			{
				for (unsigned int i=0;i<size;i+=salto)
					*(pointer+i)=1.0f;
				pointer+=bitmap->tamx;
				if (k>0)
					salto=size;
			}
			pointer+=aux1;
		}
	}
}

void FLFractalQuad3::DrawBounds(FLBitmap3& bitmap,char zoom) const
{
	for (unsigned int m=0;m<numtransformations;m++)
	{
		DrawBoundsRecurs((FLTransQuad3*)transformations[m],&bitmap,zoom);
	}
}

void FLFractalQuad3::SmoothBoundsRecurs(FLTransQuad3 *trans,FLBitmap3 *bitmap,FLBitmap3 *salida,char zoom) const
{
	/*if (trans->splitted)
	{
		SmoothBoundsRecurs(trans->child[0],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[1],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[2],bitmap,salida,zoom);
		SmoothBoundsRecurs(trans->child[3],bitmap,salida,zoom);
	}
	else
	{
		unsigned short rx,ry;
		rx=trans->ranposx<<zoom;
		ry=trans->ranposy<<zoom;

		unsigned int size=trans->ransize<<zoom;
		float w1,w2;
		float pixel1,pixel2;

		if (trans->ransize==4)
		{
			w1=5.0f;
			w2=1.0f;
		}
		else
		{
			w1=2.0f;
			w2=1.0f;
		}	

		if (ry!=0)
		{
			for (unsigned int i=0;i<size;++i) 
			{
				pixel1=bitmap->pixels[ry*bitmap->tamx+rx+i];
				pixel2=bitmap->pixels[(ry-1)*bitmap->tamx+rx+i];
				salida->pixels[ry*bitmap->tamx+rx+i]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida->pixels[(ry-1)*bitmap->tamx+rx+i]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}

		if (rx!=0)
		{
			for (unsigned int j=0;j<size;++j) 
			{
				pixel1=bitmap->pixels[(ry+j)*bitmap->tamx+rx];
				pixel2=bitmap->pixels[(ry+j)*bitmap->tamx+rx-1];
				salida->pixels[(ry+j)*bitmap->tamx+rx]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida->pixels[(ry+j)*bitmap->tamx+rx-1]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}
	}*/
}

void FLFractalQuad3::SmoothBounds(FLBitmap3& bitmap,char zoom) const
{
	/*FLBitmap2 salida(bitmap.tamx,bitmap.tamy);
	salida=bitmap;

	for (unsigned int m=0;m<numtransformations;m++)
	{
		SmoothBoundsRecurs((FLTransQuad2*)transformations[m],&bitmap,&salida,zoom);
	}

	bitmap=salida;*/
}

void FLFractalQuad3::Encode(FLBitmap3 *bitmap,unsigned short domainsalto,unsigned int nummaxtransformations)
{
	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	imagetamx=bitmap->tamx;
	imagetamy=bitmap->tamy;
	imagetamz=bitmap->tamz;

	// precalculamos los posibles dominios
	unsigned int domaintam=32;
	unsigned int firstrange=domaintam>>1;
	unsigned int lastrange=2;
	unsigned int numlevels=4;
	
	FLDomains3** domains=new FLDomains3*[numlevels]; //32,16,8,4,2?
	
	domains[0]=new FLDomains3(bitmap,domaintam,domainsalto);
	domaintam=domaintam>>1;

	// aproximamos los rangos
	numbasetrans=(imagetamz/firstrange)*(imagetamy/firstrange)*(imagetamx/firstrange);
	numtransformations=0;
	cout << "solving transformations [" << nummaxtransformations << "] = 0";
	cout.flush();
	vector<FLTrans3*> vtransformations;
	transformations=new FLTrans3*[numbasetrans];

	// creo primeros niveles hasta rangos de firstrange en orden
	FLTrans3* transaux;
	for (unsigned int k=0;k<imagetamy;k+=firstrange)
	{
		for (unsigned int j=0;j<imagetamy;j+=firstrange)
		{
			for (unsigned int i=0;i<imagetamx;i+=firstrange)
			{
				FLBitmapRef3 range(i,j,k,i,j,k,firstrange,firstrange,firstrange,bitmap);
				//range.SolveSums();

				transaux=new FLTransQuad3();
				domains[0]->OptimizeTrans(transaux,range);
	            
				vector<FLTrans3*>::iterator it=vtransformations.begin();
				while (it!=vtransformations.end())
				{
					if ((*it)->rms<transaux->rms)
						break;
					it++;
				}
				vtransformations.insert(it,transaux);

				transformations[numtransformations]=(FLTrans3*)transaux;	
				numtransformations++;

				if ((numtransformations%100)==0)
				{
					cout << "," << numtransformations;
					cout.flush();
				}
			}		
		}
	}

	cout << endl;

	delete domains[0];

	for (unsigned int h2=1;h2<numlevels;h2++)
	{
		domains[h2]=new FLDomains3(bitmap,domaintam,domainsalto);
		domaintam=domaintam>>1;
	}

	cout << "solving transformations [" << nummaxtransformations << "] = ";
	cout.flush();
	
	while(numtransformations<nummaxtransformations)
	{
		// cojo el peor
		vector<FLTrans3*>::iterator it2=vtransformations.begin();
		while (it2!=vtransformations.end())
		{
			if ((*it2)->ransize>lastrange)
				break;
			it2++;
		}
		if (it2==vtransformations.end())
		{
			cout << "all==" << lastrange;
			cout.flush();
			break;
		}
		FLTransQuad3* worsttrans=(FLTransQuad3*)(*it2);
		// lo saco del vector
		vtransformations.erase(it2);
		
		worsttrans->splitted=true;
		unsigned int numchild=0;

		unsigned int qsize=worsttrans->ransize>>1;
		unsigned char qdomain=numlevels-1;
		while(qsize>lastrange) 
		{
			qsize=qsize>>1;
			qdomain--;
		}

		// creo sus hijos y los añado ordenados
		for (unsigned short k2=worsttrans->ranposz;k2<worsttrans->ranposz+worsttrans->ransize;k2+=worsttrans->ransize>>1)
		{
			for (unsigned short j2=worsttrans->ranposy;j2<worsttrans->ranposy+worsttrans->ransize;j2+=worsttrans->ransize>>1)
			{
				for (unsigned short i2=worsttrans->ranposx;i2<worsttrans->ranposx+worsttrans->ransize;i2+=worsttrans->ransize>>1)
				{
					FLTransQuad3* transaux=new FLTransQuad3();
					
					/*if ((worsttrans->ransize>>1)==1)
					{
						FLBitmap3 rangeraw(2,2,2);
						bitmap->SuperSample2(i2,j2,k2,rangeraw);
						FLBitmapRef3 range(i2,j2,k2,0,0,0,2,2,2,&rangeraw);								
						domains[qdomain]->OptimizeTrans(transaux,range);
					}
					else*/
					{
						FLBitmapRef3 range(i2,j2,k2,i2,j2,k2,worsttrans->ransize>>1,worsttrans->ransize>>1,worsttrans->ransize>>1,bitmap);
						domains[qdomain]->OptimizeTrans(transaux,range);
					}

					//if (transaux->rms>worsttrans->rms)
					//	transaux->rms=worsttrans->rms*0.5f;
						        
					vector<FLTrans3*>::iterator it3=vtransformations.begin();
					while (it3!=vtransformations.end())
					{
						if ((*it3)->rms<transaux->rms)
							break;
						it3++;
					}
					vtransformations.insert(it3,transaux);

					worsttrans->child[numchild]=transaux;
					numchild++;
				}
			}
		}
		// libero memoria
		//delete worsttrans;
		// aumento contador
		numtransformations+=7;

		if (abs((int)numtransformations%100)<7)
		{
			cout << "," << numtransformations;
			cout.flush();
		}
	}
	if (abs((int)numtransformations%100)>=7)
		cout << "," << numtransformations;

	cout << endl;

	// vuelco transformaciones
	//transformations=new FLTrans2*[numtransformations];
	/*for (vector<FLTrans2*>::iterator it=vtransformations.begin();it!=vtransformations.end();it++)
	{
		transformations[actualtransformation]=(FLTrans2*)(*it);	
		actualtransformation++;
	}*/

	vtransformations.clear();
	
	for (unsigned int h2=1;h2<numlevels;h2++)
	{
		delete domains[h2];
	}
	delete domains;
}
void FLFractalQuad3::Save(FLFile& file) const
{
	// peor caso: 1 bit + numtransformations*48 bits 
	// mejor caso: 1 bit + numtransformations*1 bits 
	
	// type 1 bit
	// numtransformations transformaciones

	//file.Pack(8,type);
	for (unsigned int i=0;i<numbasetrans;i++)
	{
		FLTransQuad3* trans=(FLTransQuad3*)transformations[i];
		trans->Save(file);
	}
}

void FLFractalQuad3::Load(FLFile& file)
{
	// peor caso: numtransformations*48 bits 
	// mejor caso: numtransformations*1 bits 
	
	// numtransformations transformaciones

	if (transformations)
	{
		for (unsigned short i=0;i<numbasetrans;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	numbasetrans=(imagetamx/16)*(imagetamy/16)*(imagetamz/16);
	//numtransformations=(unsigned short)file.Unpack(16);
    transformations=new FLTrans3*[numbasetrans];
	numtransformations=0;

	for (unsigned short k=0;k<imagetamz;k+=16)
	{
		for (unsigned short j=0;j<imagetamy;j+=16)
		{
			for (unsigned short i=0;i<imagetamx;i+=16)
			{
				transformations[numtransformations]=new FLTransQuad3();
				((FLTransQuad3*)transformations[numtransformations])->Load(file,i,j,k,16);
				/*basetrans[numtrans].domsize=basetrans[numtrans].ransize<<1;
				basetrans[numtrans].s=(float)basetrans[numtrans].pis/(float)(1<<5)*2.0f-1.0f;
				basetrans[numtrans].o=(float)basetrans[numtrans].pio/(float)((1<<7)-1)*((1.0f+fabs(basetrans[numtrans].s))*255.0f);
				if (basetrans[numtrans].s>0.0) basetrans[numtrans].o-=basetrans[numtrans].s*255.0f;*/
				numtransformations++;
			}
		}
	}
}