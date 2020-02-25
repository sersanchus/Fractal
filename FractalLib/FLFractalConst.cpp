#include "FLFractalConst.h"
#include "FLDomains.h"
#include <math.h>
#include <iostream>

using namespace std;

FLFractalConst2::FLFractalConst2(void)
//:FLFractal2(FRACTAL_CONST)
{
	//ransize=rs;
	ransize=2;
}

FLFractalConst2::~FLFractalConst2()
{
	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
	}
}

FLBitmap2 FLFractalConst2::Decode(const FLBitmap2 &bitmap,char zoom,unsigned int npasos) const
{
	FLBitmap2 salida(bitmap.tamx,bitmap.tamy);

	FLBitmap2 *bitmap2x2[4];
	bitmap2x2[0]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
	bitmap.SubSample2(*bitmap2x2[0]);
	if (zoom==0)
	{
		FLBitmap2 bitmap2x2aux(bitmap.tamx,bitmap.tamy);
		bitmap2x2[1]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
		bitmap.SubImage(bitmap2x2aux,1,0,bitmap.tamx-2,bitmap.tamy);
		bitmap2x2aux.SubSample2(*bitmap2x2[1]);
		bitmap2x2[2]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
		bitmap.SubImage(bitmap2x2aux,0,1,bitmap.tamx,bitmap.tamy-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[2]);
		bitmap2x2[3]=new FLBitmap2(bitmap.tamx>>1,bitmap.tamy>>1);
		bitmap.SubImage(bitmap2x2aux,1,1,bitmap.tamx-2,bitmap.tamy-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[3]);
	}

	unsigned short i,j;
	unsigned short size;
	FLTrans2* trans;

	// truco: dónde queda 0
	// flip y rotate
	static int abcdef[8][6]={{1,0,0,1,0,0},{0,-1,1,0,1,0},{-1,0,0,-1,1,1},{0,1,-1,0,0,1},{-1,0,0,1,1,0},{0,-1,-1,0,1,1},{1,0,0,-1,0,1},{0,1,1,0,0,0}};
	
	int a,b,c,d,e,f;
	unsigned short posx,posy;
	unsigned short x,y;
	float *pixelpointer;

	for (unsigned int npaso=0;npaso<npasos;npaso++)
	{
 		for (unsigned int m=0;m<numtransformations;m++)
		{
			trans=transformations[m];

			size=aplicazoom(trans->ransize,zoom);

			if (trans->pis!=16)
			{
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
						salida.pixels[y*salida.tamx+x]=trans->s*(*pixelpointer)+trans->o; //level

						pixelpointer++;

						x+=a;
						y+=c;
					}
				}
			}
			else
			{
				unsigned short x=aplicazoom(trans->ranposx,zoom);
				unsigned short y=aplicazoom(trans->ranposy,zoom);

				for (j=0;j<size;j++)
					for (i=0;i<size;i++)
						salida.pixels[(y+j)*salida.tamx+(x+i)]=trans->o; //level
			}
		}
	}
	
	delete bitmap2x2[0];
	if (zoom==0)
	{
		delete bitmap2x2[1];
		delete bitmap2x2[2];
		delete bitmap2x2[3];
	}

	return salida;
}

float FLFractalConst2::Decode(const FLVector2& pos,float prec,float sem,unsigned int& deep) const
{
	FLVector2 posaux=pos;
	deep=1;
	float colorultimo=sem;
	float colorpenultimo;

	float factor0=1.0f;
    float factoracum=0.0f;
	float factorultimo=0.0f;

	//truco: igual que el otro decode pero sin matrices invertidas (si desplazamientos invertidos)
	static float abcdef[8][6]={{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f},{0.0f,1.0f,-1.0f,0.0f,1.0f,0.0f},{-1.0f,0.0f,0.0f,-1.0f,1.0f,1.0f},{0.0f,-1.0f,1.0f,0.0f,0.0f,1.0f},{-1.0f,0.0f,0.0f,1.0f,1.0f,0.0f},{0.0f,-1.0f,-1.0f,0.0f,1.0f,1.0f},{1.0f,0.0f,0.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f,1.0f,0.0f,0.0f,0.0f}};
	float posxf,posyf;
	unsigned short posx,posy;
	float a,b,c,d,e,f;
	float relx,rely;
	FLTrans2* trans;

	do
	{
		if (posaux.x<0.0f) posaux.x=0.0f;
		if (posaux.y<0.0f) posaux.y=0.0f;
		if (posaux.x>1.0f) posaux.x=1.0f;
		if (posaux.y>1.0f) posaux.y=1.0f;

		// calculo punto a transformar
		posxf=posaux.x*(float)(imagetamx);
		posyf=posaux.y*(float)(imagetamy);
		posx=(unsigned short)(posxf*0.99999f);
		posy=(unsigned short)(posyf*0.99999f);
	
		// busco en que trasformación caeria posx,posy
		trans=transformations[(posy/ransize)*(imagetamx/ransize)+(posx/ransize)];

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
		
			factoracum+=factorultimo;
			factorultimo=factor0*trans->o;
			factor0*=trans->s;
			
			colorultimo=sem*factor0+factoracum+factorultimo;
		}
		else
		{
			factoracum+=factorultimo;
			factorultimo=factor0*trans->o;
			colorultimo=factoracum+factorultimo;

			return colorultimo;
		}	
	
		deep++;
	}
	while(deep<4 || (deep<51 && fabs(colorpenultimo-colorultimo)>prec));

	return colorultimo;
}

/*float DecodeRecurs(const FLVector2& pos,unsigned int deep,FLTrans2** transformations,float& factor0, float& factoracum, float& factorultimo)
{
	FLVector2 posaux=pos;
	
	if (posaux.x<0.0f) posaux.x=0.0f;
	if (posaux.y<0.0f) posaux.y=0.0f;
	if (posaux.x>1.0f) posaux.x=1.0f;
	if (posaux.y>1.0f) posaux.y=1.0f;

	// calculo punto a transformar
	float posxf=posaux.x*(float)(imagetamx);
	float posyf=posaux.y*(float)(imagetamy);
	unsigned short posx=(unsigned short)(posxf*0.99999f);
	unsigned short posy=(unsigned short)(posyf*0.99999f);

	// busco en que trasformación caeria posx,posy
	unsigned int m=(posy/ransize)*(imagetamx/ransize)+(posx/ransize);

	// aplico transformación
	if (transformations[m]->pis!=16)

	for (int j=-1;j<1;j+=2)
	{
		float posyf2=posyf+j*
		unsigned short posy=(unsigned short)(posyf*0.99999f);

		for (int i=-1;i<1;i+=2)
		{
			float posxf2=posxf+i*
			unsigned short posx=(unsigned short)(posxf*0.99999f);	

			// busco en que trasformación caeria posx,posy
			unsigned int m=(posy/ransize)*(imagetamx/ransize)+(posx/ransize);

			// aplico transformación
			if (transformations[m]->pis!=16)
			{
				float a,b,c,d;
				float e,f;
				float aux1=(float)transformations[m]->ransize;
				switch(transformations[m]->ori)
				{
					//truco: igual que el otro decode pero sin matrices invertidas (si desplazamientos invertidos)
					case 0:
						a=1.0f;
						b=0.0f;
						c=0.0f;
						d=1.0f;
						e=0.0f;
						f=0.0f;
						break;
					case 1:
						a=0.0f;
						b=1.0f;
						c=-1.0f;
						d=0.0f;
						e=aux1;
						f=0.0f;
						break;
					case 2:
						a=-1.0f;
						b=0.0f;
						c=0.0f;
						d=-1.0f;
						e=aux1;
						f=aux1;
						break;
					case 3:
						a=0.0f;
						b=-1.0f;
						c=1.0f;
						d=0.0f;
						e=0.0f;
						f=aux1;
						break;
					case 4:
						a=-1.0f;
						b=0.0f;
						c=0.0f;
						d=1.0f;
						e=aux1;
						f=0.0f;
						break;
					case 5:
						a=0.0f;
						b=-1.0f;
						c=-1.0f;
						d=0.0f;
						e=aux1;
						f=aux1;
						break;
					case 6:
						a=1.0f;
						b=0.0f;
						c=0.0f;
						d=-1.0f;
						e=0.0f;
						f=aux1;
						break;
					case 7:
						a=0.0f;
						b=1.0f;
						c=1.0f;
						d=0.0f;
						e=0.0f;
						f=0.0f;
						break;
				}

				float relx=posxf-e-(float)transformations[m]->ranposx;
				float rely=posyf-f-(float)transformations[m]->ranposy;

				posaux.x=a*relx+b*rely;
				posaux.y=c*relx+d*rely;

				posaux.x*=2.0f;
				posaux.y*=2.0f;

				posaux.x+=(float)transformations[m]->domposx;
				posaux.y+=(float)transformations[m]->domposy;
				
				posaux.x/=(float)(imagetamx);
				posaux.y/=(float)(imagetamy);

				colorpenultimo=colorultimo;
			
				factoracum+=factorultimo;
				factorultimo=factor0*transformations[m]->o;
				factor0*=transformations[m]->s;
				
				colorultimo=sem*factor0+factoracum+factorultimo;
			}
			else
			{
				factoracum+=factorultimo;
				factorultimo=factor0*transformations[m]->o;
				colorultimo=factoracum+factorultimo;

				return colorultimo;
			}	
		}
	}
}

float FLFractalConst2::Decode(const FLVector2& pos,float prec,float sem,unsigned int& deep) const
{
	FLVector2 posaux=pos;
	deep=1;
	float colorultimo=sem;
	float colorpenultimo;

	float factor0=1.0f;
    float factoracum=0.0f;
	float factorultimo=0.0f;

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
	
		// busco en que trasformación caeria posx,posy
		unsigned int m=(posy/ransize)*(imagetamx/ransize)+(posx/ransize);

		// aplico transformación
		if (transformations[m]->pis!=16)
		{
			float a,b,c,d;
			float e,f;
			float aux1=(float)transformations[m]->ransize;
			switch(transformations[m]->ori)
			{
				//truco: igual que el otro decode pero sin matrices invertidas (si desplazamientos invertidos)
				case 0:
					a=1.0f;
					b=0.0f;
					c=0.0f;
					d=1.0f;
					e=0.0f;
					f=0.0f;
					break;
				case 1:
					a=0.0f;
					b=1.0f;
					c=-1.0f;
					d=0.0f;
					e=aux1;
					f=0.0f;
					break;
				case 2:
					a=-1.0f;
					b=0.0f;
					c=0.0f;
					d=-1.0f;
					e=aux1;
					f=aux1;
					break;
				case 3:
					a=0.0f;
					b=-1.0f;
					c=1.0f;
					d=0.0f;
					e=0.0f;
					f=aux1;
					break;
				case 4:
					a=-1.0f;
					b=0.0f;
					c=0.0f;
					d=1.0f;
					e=aux1;
					f=0.0f;
					break;
				case 5:
					a=0.0f;
					b=-1.0f;
					c=-1.0f;
					d=0.0f;
					e=aux1;
					f=aux1;
					break;
				case 6:
					a=1.0f;
					b=0.0f;
					c=0.0f;
					d=-1.0f;
					e=0.0f;
					f=aux1;
					break;
				case 7:
					a=0.0f;
					b=1.0f;
					c=1.0f;
					d=0.0f;
					e=0.0f;
					f=0.0f;
					break;
			}


			float relx=posxf-e-(float)transformations[m]->ranposx;
			float rely=posyf-f-(float)transformations[m]->ranposy;

			posaux.x=a*relx+b*rely;
			posaux.y=c*relx+d*rely;

			posaux.x*=2.0f;
			posaux.y*=2.0f;

			posaux.x+=(float)transformations[m]->domposx;
			posaux.y+=(float)transformations[m]->domposy;
			
			posaux.x/=(float)(imagetamx);
			posaux.y/=(float)(imagetamy);

			colorpenultimo=colorultimo;
		
			factoracum+=factorultimo;
			factorultimo=factor0*transformations[m]->o;
			factor0*=transformations[m]->s;
			
			colorultimo=sem*factor0+factoracum+factorultimo;
		}
		else
		{
			factoracum+=factorultimo;
			factorultimo=factor0*transformations[m]->o;
			colorultimo=factoracum+factorultimo;

			return colorultimo;
		}	
	
		deep++;
	}
	while(deep<4 || (deep<51 && fabs(colorpenultimo-colorultimo)>prec));

	return colorultimo;
}*/

void FLFractalConst2::DrawBounds(FLBitmap2& bitmap,char zoom) const
{
	unsigned short size=aplicazoom(ransize,zoom);
	float *pointer=bitmap.pixels;
	for (unsigned int j=0;j<bitmap.tamy;j++)
	{
		for (unsigned int i=0;i<bitmap.tamx;i++)
		{
			if ((i%size)==0 || (j%size)==0)
				*pointer=1.0f;
			pointer++;
		}
	}
}

void FLFractalConst2::SmoothBounds(FLBitmap2& bitmap,char zoom) const
{
	FLBitmap2 salida(bitmap.tamx,bitmap.tamy);
	salida=bitmap;

	unsigned short size=aplicazoom(ransize,zoom);
	float w1=5.0f;
	float w2=1.0f;
	float pixel1,pixel2;
	unsigned short rx,ry;

	for (unsigned int m=0;m<numtransformations;m++)
	{
		rx=aplicazoom(transformations[m]->ranposx,zoom);
		ry=aplicazoom(transformations[m]->ranposy,zoom);

		if (ry!=0)
		{
			for (unsigned int i=0;i<size;++i) 
			{
				pixel1=bitmap.pixels[ry*bitmap.tamx+rx+i];
				pixel2=bitmap.pixels[(ry-1)*bitmap.tamx+rx+i];
				salida.pixels[ry*bitmap.tamx+rx+i]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida.pixels[(ry-1)*bitmap.tamx+rx+i]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}

		if (rx!=0)
		{
			for (unsigned int j=0;j<size;++j) 
			{
				pixel1=bitmap.pixels[(ry+j)*bitmap.tamx+rx];
				pixel2=bitmap.pixels[(ry+j)*bitmap.tamx+rx-1];
				salida.pixels[(ry+j)*bitmap.tamx+rx]=(w1*pixel1+w2*pixel2)/(w1+w2);
				salida.pixels[(ry+j)*bitmap.tamx+rx-1]=(w2*pixel1+w1*pixel2)/(w1+w2);
			}
		}
	}

	bitmap=salida;
}

void FLFractalConst2::Encode(FLBitmap2 *bitmap,unsigned short domainsalto,unsigned short rs)
{
	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	imagetamx=bitmap->tamx;
	imagetamy=bitmap->tamy;

	// tamaño de rango
	ransize=rs;

	FLDomains2 domains(bitmap,ransize<<1,domainsalto);
	
	// creo espacio para transformaciones
	numtransformations=(imagetamx*imagetamy)/(ransize*ransize);
	transformations=new FLTrans2*[numtransformations];

	// aproximamos los rangos
	FLTrans2* transaux;
	cout << "solving ranges of " << ransize << " [" << numtransformations << "] = 0";
	cout.flush();
	unsigned int cont=0;
	for (int j=0;j<imagetamy;j+=ransize)
	{
		for (int i=0;i<imagetamx;i+=ransize)
		{
			transaux=transformations[cont]=new FLTrans2();

			if (ransize==1)
			{
				FLBitmap2 rangeraw(2,2);
				bitmap->SuperSample2(i,j,rangeraw);
				FLBitmapRef2 range(i,j,0,0,2,2,&rangeraw);								
				domains.OptimizeTrans(transaux,range);
			}
			else
			{
				FLBitmapRef2 range(i,j,i,j,ransize,ransize,bitmap);
				domains.OptimizeTrans(transaux,range);
			}
			
			cont++;
		}
		cout << "," << cont;
		cout.flush();
	}
	cout << endl;
}

void FLFractalConst2::Save(FLFile& file) const
{
	// peor caso: 3 bytes + numtransformations*47 bits 
	// mejor caso: 3 bytes + numtransformations*12 bits 
	
	// tipo 1 byte
	// ransize 2 bytes
	// numtransformations=(imagetamx*imagetamy)/(ransize*ransize) transformaciones

	FLTransConst2* transaux;
	
	file.Pack(16,ransize); // 2 bytes ransize
	for (unsigned int i=0;i<numtransformations;i++)
	{
		transaux=((FLTransConst2*)transformations[i]);
		transaux->Save(file);
	}
}

void FLFractalConst2::Load(FLFile& file)
{
    // peor caso: 2 bytes + numtransformations*47 bits 
	// mejor caso: 2 bytes + numtransformations*12 bits 
	
	// ransize 2 bytes
	// numtransformations=(imagetamx*imagetamy)/(ransize*ransize) transformaciones

	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	unsigned short transx,transy;
	ransize=(unsigned short)file.Unpack(16);
	transx=imagetamx/ransize;	
	transy=imagetamy/ransize;	
	numtransformations=transx*transy;
	transformations=new FLTrans2*[numtransformations];
	unsigned int numtrans=0;
	for (unsigned short j=0;j<imagetamy;j+=ransize)
	{
		for (unsigned short i=0;i<imagetamx;i+=ransize)
		{
			transformations[numtrans]=new FLTransConst2();
			((FLTransConst2*)transformations[numtrans])->Load(file);
			transformations[numtrans]->ranposx=i;
			transformations[numtrans]->ranposy=j;
			transformations[numtrans]->ransize=ransize;
			transformations[numtrans]->domsize=ransize<<1;
			//transformations[numtrans]->s=(float)transformations[numtrans]->pis/(float)(1<<5)*2.0f-1.0f;
			transformations[numtrans]->s=(float)transformations[numtrans]->pis*0.0625f-1.0f;
	        //transformations[numtrans]->o=(float)transformations[numtrans]->pio/(float)((1<<7)-1)*((1.0f+fabs(transformations[numtrans]->s))*255.0f);
			transformations[numtrans]->o=(float)transformations[numtrans]->pio*0.0078740157480314f*(1.0f+fabs(transformations[numtrans]->s));
			if (transformations[numtrans]->s>0.0) 
				transformations[numtrans]->o-=transformations[numtrans]->s;
			numtrans++;
		}
	}
}

FLFractalConst3::FLFractalConst3(void)
//:FLFractal2(FRACTAL_CONST)
{
	//ransize=rs;
	ransize=2;
}

FLFractalConst3::~FLFractalConst3()
{
	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
	}
}

FLBitmap3 FLFractalConst3::Decode(const FLBitmap3 &bitmap,char zoom,unsigned int npasos) const
{
	FLBitmap3 salida(bitmap.tamx,bitmap.tamy,bitmap.tamz);

	FLBitmap3 *bitmap2x2[8];
	FLBitmap3 bitmap2x2aux(bitmap.tamx,bitmap.tamy,bitmap.tamz);
	bitmap2x2[0]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
	bitmap.SubSample2(*bitmap2x2[0]);
	if (zoom==0)
	{
		bitmap2x2[1]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,1,0,0,bitmap.tamx-2,bitmap.tamy,bitmap.tamz);
		bitmap2x2aux.SubSample2(*bitmap2x2[1]);
		bitmap2x2[2]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,0,1,0,bitmap.tamx,bitmap.tamy-2,bitmap.tamz);
		bitmap2x2aux.SubSample2(*bitmap2x2[2]);
		bitmap2x2[3]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,1,1,0,bitmap.tamx-2,bitmap.tamy-2,bitmap.tamz);
		bitmap2x2aux.SubSample2(*bitmap2x2[3]);
		bitmap2x2[4]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,0,0,1,bitmap.tamx,bitmap.tamy,bitmap.tamz-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[4]);
		bitmap2x2[5]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,1,0,1,bitmap.tamx-2,bitmap.tamy,bitmap.tamz-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[5]);
		bitmap2x2[6]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,0,1,1,bitmap.tamx,bitmap.tamy-2,bitmap.tamz-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[6]);
		bitmap2x2[7]=new FLBitmap3(bitmap.tamx>>1,bitmap.tamy>>1,bitmap.tamz>>1);
		bitmap.SubImage(bitmap2x2aux,1,1,1,bitmap.tamx-2,bitmap.tamy-2,bitmap.tamz-2);
		bitmap2x2aux.SubSample2(*bitmap2x2[7]);
	}
	
	unsigned short i,j,k;
	unsigned short size;
	FLTrans3* trans;
	
	for (unsigned int npaso=0;npaso<npasos;npaso++)
	{
 		for (unsigned int m=0;m<numtransformations;m++)
		{
			trans=transformations[m];

			size=aplicazoom(trans->ransize,zoom);

			if (trans->pis!=16)
			{
				int m[9];
				int dx,dy,dz;
				int aux1=size-1;
				dx=aplicazoom(trans->ranposx,zoom);
				dy=aplicazoom(trans->ranposy,zoom);
				dz=aplicazoom(trans->ranposz,zoom);

				// truco: dónde queda 0
				// flip y rotate

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
				FLBitmap3* auxref2x2=bitmap2x2[((posz&1)<<2)|((posy&1)<<1)|(posx&1)];
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

						pixelpointer=auxref2x2->pixels+(k+posz)*auxref2x2->tamx*auxref2x2->tamy+(j+posy)*auxref2x2->tamx+posx;
						for (i=0;i<size;i++)
						{
							salida.pixels[z*salida.tamy*salida.tamx+y*salida.tamx+x]=trans->s*(*pixelpointer)+trans->o; //level

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
					for (j=0;j<size;j++)
						for (i=0;i<size;i++)
							salida.pixels[(z+k)*salida.tamy*salida.tamx+(y+j)*salida.tamx+(x+i)]=trans->o; //level
			}
		}
	}
	
	for (int i=0;i<((zoom==0)?8:1);i++)
		delete bitmap2x2[i];

	return salida;
}

float FLFractalConst3::Decode(const FLVector3& pos,float prec,float sem,unsigned int& deep) const
{
	FLVector3 posaux=pos;
	//unsigned int deep=1;
	deep=1;
	//float factores[256];
	float colorultimo=sem;
	float colorpenultimo;
	//unsigned int ij;
	//float ultimos;
	//float ultimoo;
   
	//memset(factores,0,1024);
	//factores[0]=1.0f;

	float factor0=1.0f;
    float factoracum=0.0f;
	float factorultimo=0.0f;

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
		unsigned int numtrans=(posz/ransize)*(imagetamy/ransize)*(imagetamx/ransize)+(posy/ransize)*(imagetamx/ransize)+(posx/ransize);

		// aplico transformación
		if (transformations[numtrans]->pis!=16)
		{
			float m[9];
			float dx,dy,dz;
			float aux1=(float)transformations[numtrans]->ransize;
			switch(transformations[numtrans]->ori)
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

			float relx=posxf-dx-(float)transformations[numtrans]->ranposx;
			float rely=posyf-dy-(float)transformations[numtrans]->ranposy;
			float relz=poszf-dz-(float)transformations[numtrans]->ranposz;

			posaux.x=m[0]*relx+m[3]*rely+m[6]*relz;
			posaux.y=m[1]*relx+m[4]*rely+m[7]*relz;
			posaux.z=m[2]*relx+m[5]*rely+m[8]*relz;

			posaux.x*=2.0f;
			posaux.y*=2.0f;
			posaux.z*=2.0f;

			posaux.x+=(float)transformations[numtrans]->domposx;
			posaux.y+=(float)transformations[numtrans]->domposy;
			posaux.z+=(float)transformations[numtrans]->domposz;
			
			posaux.x/=(float)(imagetamx);
			posaux.y/=(float)(imagetamy);
			posaux.z/=(float)(imagetamz);
		
			//if (deep==1)
			//{
			//	ultimos=transformations[m]->s;
			//	ultimoo=transformations[m]->o;
			//}
			colorpenultimo=colorultimo;
			//colorultimo=sem;
			
			factoracum+=factorultimo;
			factorultimo=factor0*transformations[numtrans]->o;
			factor0*=transformations[numtrans]->s;
			
			colorultimo=sem*factor0+factoracum+factorultimo;
		}
		else
		{
			//return transformations[numtrans]->o;

			factoracum+=factorultimo;
			factorultimo=factor0*transformations[numtrans]->o;
			colorultimo=factoracum+factorultimo;

			return colorultimo;
		}	
		
		//if (colorultimo<0.0f)
		//	colorultimo=0.0f;
		//else if (colorultimo>255.0f)
		//	colorultimo=255.0f;

		//colorpenultimo=(colorultimo-ultimoo)/ultimos;
		
		//if (colorpenultimo<0.0f)
		//	colorpenultimo=0.0f;
		//else if (colorpenultimo>255.0f)
		//	colorpenultimo=255.0f;
		
		deep++;
	}
	while(deep<4 || (deep<51 && fabs(colorpenultimo-colorultimo)>prec));

	//if (colorultimo<0.0f)
	//	colorultimo=0.0f;
	//else if (colorultimo>255.0f)
	//	colorultimo=255.0f;

	return colorultimo;
}

void FLFractalConst3::DrawBounds(FLBitmap3& bitmap,char zoom) const
{
	unsigned int size=aplicazoom(ransize,zoom);
	float *pointer=bitmap.pixels;
	for (unsigned int k=0;k<bitmap.tamz;k++)
	{
		for (unsigned int j=0;j<bitmap.tamy;j++)
		{
			for (unsigned int i=0;i<bitmap.tamx;i++)
			{
				if ((i%size)==0 || (j%size)==0 || (k%size)==0)
					*pointer=1.0f;
				pointer++;
			}
		}
	}
}

void FLFractalConst3::SmoothBounds(FLBitmap3& bitmap,char zoom) const
{
	FLBitmap3 salida(bitmap.tamx,bitmap.tamy,bitmap.tamz);
	salida=bitmap;

	unsigned int size=aplicazoom(ransize,zoom);
	float w1=5.0f;
	float w2=1.0f;
	float pixel1,pixel2;
	unsigned short rx,ry,rz;

	for (unsigned int m=0;m<numtransformations;m++)
	{
		rx=aplicazoom(transformations[m]->ranposx,zoom);
		ry=aplicazoom(transformations[m]->ranposy,zoom);
		rz=aplicazoom(transformations[m]->ranposz,zoom);

		for (unsigned int k=0;k<size;++k) 
		{
			if (ry!=0)
			{
				for (unsigned int i=0;i<size;++i) 
				{
					pixel1=bitmap.pixels[(rz+k)*bitmap.tamxpertamy+ry*bitmap.tamx+rx+i];
					pixel2=bitmap.pixels[(rz+k)*bitmap.tamxpertamy+(ry-1)*bitmap.tamx+rx+i];
					salida.pixels[(rz+k)*bitmap.tamxpertamy+ry*bitmap.tamx+rx+i]=(w1*pixel1+w2*pixel2)/(w1+w2);
					salida.pixels[(rz+k)*bitmap.tamxpertamy+(ry-1)*bitmap.tamx+rx+i]=(w2*pixel1+w1*pixel2)/(w1+w2);
				}
			}

			if (rx!=0)
			{
				for (unsigned int j=0;j<size;++j) 
				{
					pixel1=bitmap.pixels[(rz+k)*bitmap.tamxpertamy+(ry+j)*bitmap.tamx+rx];
					pixel2=bitmap.pixels[(rz+k)*bitmap.tamxpertamy+(ry+j)*bitmap.tamx+rx-1];
					salida.pixels[(rz+k)*bitmap.tamxpertamy+(ry+j)*bitmap.tamx+rx]=(w1*pixel1+w2*pixel2)/(w1+w2);
					salida.pixels[(rz+k)*bitmap.tamxpertamy+(ry+j)*bitmap.tamx+rx-1]=(w2*pixel1+w1*pixel2)/(w1+w2);
				}
			}
		}
	}

	bitmap=salida;
}

void FLFractalConst3::Encode(FLBitmap3 *bitmap,unsigned short domainsalto,unsigned short rs)
{
	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	imagetamx=bitmap->tamx;
	imagetamy=bitmap->tamy;
	imagetamz=bitmap->tamz;

	// tamaño de rango
	ransize=rs;

	FLDomains3 domains(bitmap,ransize<<1,domainsalto);
	
	// creo espacio para transformaciones
	numtransformations=(imagetamx*imagetamy*imagetamz)/(ransize*ransize*ransize);
	transformations=new FLTrans3*[numtransformations];

	// aproximamos los rangos
	FLTrans3* transaux;
	cout << "solving ranges of " << ransize << " [" << numtransformations << "] = 0";
	cout.flush();
	unsigned int cont=0;
	for (int k=0;k<imagetamz;k+=ransize)
	{
		for (int j=0;j<imagetamy;j+=ransize)
		{
			for (int i=0;i<imagetamx;i+=ransize)
			{
				transaux=transformations[cont]=new FLTrans3();

				if (ransize==1)
				{
					FLBitmap3 rangeraw(2,2,2);
					bitmap->SuperSample2(i,j,k,rangeraw);
					FLBitmapRef3 range(i,j,k,0,0,0,2,2,2,&rangeraw);								
					domains.OptimizeTrans(transaux,range);
				}
				else
				{
					FLBitmapRef3 range(i,j,k,i,j,k,ransize,ransize,ransize,bitmap);
					domains.OptimizeTrans(transaux,range);
				}

				cont++;
			}
			cout << "," << cont;
			cout.flush();
		}
	}
	cout << endl;
}

void FLFractalConst3::Save(FLFile& file) const
{
	// peor caso: 3 bytes + numtransformations*47 bits 
	// mejor caso: 3 bytes + numtransformations*12 bits 
	
	// tipo 1 byte
	// ransize 2 bytes
	// numtransformations=(imagetamx*imagetamy)/(ransize*ransize) transformaciones

	/*FLFile file2("fractalshader-dompos.raw","w+b");
	FLFile file3("fractalshader-so.raw","w+b");
	FLFile file4("fractalshader-ori.raw","w+b");*/

	//file.Pack(8,type);
	file.Pack(16,ransize); // 2 bytes ransize
	for (unsigned int i=0;i<numtransformations;i++)
	{
		((FLTransConst3*)transformations[i])->Save(file);

		/*file2.Pack(16,((FLTransConst*)transformations[i])->domposx);
		file2.Pack(16,((FLTransConst*)transformations[i])->domposy);
		file3.Pack(32,*(long*)&((FLTransConst*)transformations[i])->s);
		file3.Pack(32,*(long*)&((FLTransConst*)transformations[i])->o);
		file4.Pack(8,((FLTransConst*)transformations[i])->ori);*/
	}

	/*file2.Close();
	file3.Close();
	file4.Close();*/
}

void FLFractalConst3::Load(FLFile& file)
{
    // peor caso: 2 bytes + numtransformations*47 bits 
	// mejor caso: 2 bytes + numtransformations*12 bits 
	
	// ransize 2 bytes
	// numtransformations=(imagetamx*imagetamy)/(ransize*ransize) transformaciones

	if (transformations)
	{
		for (unsigned int i=0;i<numtransformations;i++)
			delete transformations[i];
		delete[] transformations;
		numtransformations=0;
	}

	unsigned short transx,transy,transz;
	ransize=(unsigned short)file.Unpack(16);
	transx=imagetamx/ransize;	
	transy=imagetamy/ransize;	
	transz=imagetamz/ransize;	
	numtransformations=transx*transy*transz;
	transformations=new FLTrans3*[numtransformations];
	unsigned int numtrans=0;
	for (unsigned short k=0;k<imagetamz;k+=ransize)
	{
		for (unsigned short j=0;j<imagetamy;j+=ransize)
		{
			for (unsigned short i=0;i<imagetamx;i+=ransize)
			{
				transformations[numtrans]=new FLTransConst3();
				((FLTransConst3*)transformations[numtrans])->Load(file);
				transformations[numtrans]->ranposx=i;
				transformations[numtrans]->ranposy=j;
				transformations[numtrans]->ranposz=k;
				transformations[numtrans]->ransize=ransize;
				transformations[numtrans]->domsize=ransize<<1;
				//transformations[numtrans]->s=(float)transformations[numtrans]->pis/(float)(1<<5)*2.0f-1.0f;
				transformations[numtrans]->s=(float)transformations[numtrans]->pis*0.0625f-1.0f;
				//transformations[numtrans]->o=(float)transformations[numtrans]->pio/(float)((1<<7)-1)*((1.0f+fabs(transformations[numtrans]->s))*255.0f);
				transformations[numtrans]->o=(float)transformations[numtrans]->pio*0.0078740157480314f*(1.0f+fabs(transformations[numtrans]->s));
				if (transformations[numtrans]->s>0.0) 
					transformations[numtrans]->o-=transformations[numtrans]->s;
				numtrans++;
			}
		}
	}
}