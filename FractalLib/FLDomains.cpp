#include "FLDomains.h"
#include <math.h>
#include <iostream>

bool FLDomains2::AddIfDifferent(FLBitmapRef2* domainaux)
{
	static unsigned int sumsaux;
	static vector<FLBitmapRef2*>::iterator it;
	static vector<FLBitmapRef2*>::iterator itend;
	static map<float,vector<FLBitmapRef2*> >::iterator itprimerpixel;
	static float pixelvalue;

	sumsaux=domainaux->SolveSums();
	pixelvalue=domainaux->Pixel(0,0);
	itprimerpixel=domainorder.find(pixelvalue);
	if (itprimerpixel==domainorder.end())
	{
		vector<FLBitmapRef2*> auxvector;
		auxvector.push_back(domainaux);
		domainorder.insert(pair<float,vector<FLBitmapRef2*> >(pixelvalue,auxvector));
		domain[sumsaux].push_back(domainaux);
		return true;
	}
	it=itprimerpixel->second.begin();
	itend=itprimerpixel->second.end();
	for (;it!=itend;it++)
	{
		if ((**it)==(*domainaux))
		{
			delete domainaux;
			return false;
		}
	}
	itprimerpixel->second.push_back(domainaux);
	domain[sumsaux].push_back(domainaux);
	return true;
}

FLDomains2::FLDomains2(FLBitmap2* bitmap,unsigned short domsize2,unsigned short domainsalto)
{
	domsize=domsize2;
	domsalto=domainsalto;
	
	unsigned int numdoms=0;
	
	unsigned short ransize=domsize>>1;

	if (ransize==1)
	{
		bitmap2x2[0]=new FLBitmap2(*bitmap);
		bitmap2x2[1]=bitmap2x2[2]=bitmap2x2[3]=NULL;

		// cojo los dominios de 2 sin subsampling

		FLBitmapRef2 *domainaux;

		unsigned int domainstamx=(bitmap2x2[0]->tamx-domsize)/domainsalto+1;
		unsigned int domainstamy=(bitmap2x2[0]->tamy-domsize)/domainsalto+1;
		unsigned int domainstam=domainstamx*domainstamy*8;

		cout << "solving domains of " << domsize << " [" << domainstam << "] ... ";
		cout.flush();

		unsigned short i2,j2;
		unsigned short i2fin,j2fin;
		j2fin=bitmap2x2[0]->tamy-domsize;
		i2fin=bitmap2x2[0]->tamx-domsize;
		unsigned short desp=domsize-1;
		for (j2=0;j2<=j2fin;j2+=domainsalto)
		{
			for (i2=0;i2<=i2fin;i2+=domainsalto)
			{
				// truco: quién es el que queda en 0,0

				// si el dominio sin rotación ya existe descartamos 
				// aún podrían darse dominios repetidos si los originales son rotaciones de alguno dado

				domainaux=new FLBitmapRef2(i2,j2,i2,j2,domsize,domsize,bitmap2x2[0]);
				if (!AddIfDifferent(domainaux))	continue;
				// los rotates
				domainaux=new FLBitmapRef2(i2,j2,i2,j2+desp,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(1);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2+desp,j2+desp,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(2);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2+desp,j2,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(3);
				domain[domainaux->SolveSums()].push_back(domainaux);
				// los flips
				domainaux=new FLBitmapRef2(i2,j2,i2+desp,j2,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(4);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2+desp,j2+desp,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(5);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2,j2+desp,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(6);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2,j2,domsize,domsize,bitmap2x2[0]);
				domainaux->SetOrientation(7);
				domain[domainaux->SolveSums()].push_back(domainaux);

				numdoms+=8;
			}
		}
		cout << numdoms << endl;
	}
	else
	{
		bitmap2x2[0]=new FLBitmap2(bitmap->tamx>>1,bitmap->tamy>>1);
		bitmap->SubSample2(*bitmap2x2[0]);

		if ((domainsalto%2)!=0)
		{
			FLBitmap2 bitmap2x2aux(bitmap->tamx,bitmap->tamy);
			bitmap2x2[1]=new FLBitmap2(bitmap->tamx>>1,bitmap->tamy>>1);
			bitmap->SubImage(bitmap2x2aux,1,0,bitmap->tamx-2,bitmap->tamy);
			bitmap2x2aux.SubSample2(*bitmap2x2[1]);
			bitmap2x2[2]=new FLBitmap2(bitmap->tamx>>1,bitmap->tamy>>1);
			bitmap->SubImage(bitmap2x2aux,0,1,bitmap->tamx,bitmap->tamy-2);
			bitmap2x2aux.SubSample2(*bitmap2x2[2]);
			bitmap2x2[3]=new FLBitmap2(bitmap->tamx>>1,bitmap->tamy>>1);
			bitmap->SubImage(bitmap2x2aux,1,1,bitmap->tamx-2,bitmap->tamy-2);
			bitmap2x2aux.SubSample2(*bitmap2x2[3]);
		}

		FLBitmapRef2 *domainaux;

		unsigned int domainstamx=(bitmap->tamx-domsize)/domainsalto+1;
		unsigned int domainstamy=(bitmap->tamy-domsize)/domainsalto+1;
		unsigned int domainstam=domainstamx*domainstamy*8;

		cout << "solving domains of " << domsize << " [" << domainstam << "] ... ";
		cout.flush();

		unsigned short i2,j2;
		unsigned short i2fin,j2fin;
		j2fin=bitmap->tamy-domsize;
		i2fin=bitmap->tamx-domsize;
		unsigned short i2div2,j2div2;
		FLBitmap2* bitmapaux;
		unsigned short desp=ransize-1;
		for (j2=0;j2<=j2fin;j2+=domainsalto)
		{
			j2div2=j2>>1;
			for (i2=0;i2<=i2fin;i2+=domainsalto)
			{
				i2div2=i2>>1;
				bitmapaux=bitmap2x2[((j2&1)<<1)|(i2&1)];

				// truco: quién es el que queda en 0,0
				
				// si el dominio sin rotación ya existe descartamos 
				// aún podrían darse dominios repetidos si los originales son rotaciones de alguno dado

				domainaux=new FLBitmapRef2(i2,j2,i2div2,j2div2,ransize,ransize,bitmapaux);
				if (!AddIfDifferent(domainaux))	continue;
				// los rotates
				domainaux=new FLBitmapRef2(i2,j2,i2div2,j2div2+desp,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(1);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2div2+desp,j2div2+desp,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(2);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2div2+desp,j2div2,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(3);
				domain[domainaux->SolveSums()].push_back(domainaux);
				// los flips
				domainaux=new FLBitmapRef2(i2,j2,i2div2+desp,j2div2,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(4);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2div2+desp,j2div2+desp,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(5);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2div2,j2div2+desp,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(6);
				domain[domainaux->SolveSums()].push_back(domainaux);
				domainaux=new FLBitmapRef2(i2,j2,i2div2,j2div2,ransize,ransize,bitmapaux);
				domainaux->SetOrientation(7);
				domain[domainaux->SolveSums()].push_back(domainaux);

				numdoms+=8;
			}
		}
		cout << numdoms << endl;
	}
}

FLDomains2::~FLDomains2(void)
{
	int i;

	for (i=0;i<(((domsalto%2)!=0)?4:1);i++)
		if (bitmap2x2[i]) delete bitmap2x2[i];

	vector<FLBitmapRef2*>::iterator it;
	for (i=0;i<24;i++)
	{
		for (it=domain[i].begin();it!=domain[i].end();it++)
			delete *it;
		domain[i].clear();		
	}
	map<float,vector<FLBitmapRef2*> >::iterator it2;
	for (it2=domainorder.begin();it2!=domainorder.end();it2++)
		(*it2).second.clear();
}

//#include <mm3dnow.h>
//#define _m_from_floats(f)   (*(__m64 *)(f))

inline float FloatInverse(float f)
{
	float auxf;
	int auxi=0x7F000000-*(int*)&f;
	auxf=*(float*)&auxi;
	return auxf*(2.0f-f*auxf);
}

inline int FloatToInt(float f)
{
	float aux=f+12582912.0f;
	return (*(int*)&aux)-1262485504;
}

float LeastSquares2(const FLBitmapRef2& d, const FLBitmapRef2& r, float& s2, float& o2, int& pis2, int& pio2)
{
	float n=(float)d.numpixels;
	
	//float onedivn=1.0f/n;
	float onedivn=FloatInverse(n);
	
	float denom=n*d.sumsqri-d.sumi*d.sumi;
		
	if (fabs(denom)<0.000001) // denom==0
	{
		float o2aux;
		int pio2aux;

		pis2=16;
		s2=0.0f;

		o2aux=r.sumi*onedivn;
		
		// calculamos pio2
		// se transforma 0,1 en 0,127
		pio2aux=FloatToInt(o2aux*127.0f);
		
		if (pio2aux<0) pio2aux=0;
		else if (pio2aux>=128) pio2aux=127;
		
		// recalculamos o2 con nuevo valor discreto
		o2aux=(float)pio2aux*0.007874015748031f;

		o2=o2aux;
		pio2=pio2aux;

		return /*sqrtf*/fabs(r.sumsqri+o2aux*(o2aux*n-2.0f*r.sumi))*onedivn;
	}
	else
	{
		float s2aux,o2aux;
		float faux1;
		int pis2aux,pio2aux;
		
		register float sumdri=0.0f;
		

		// sin optimizar
		for (unsigned int j=0;j<d.tamy;j++)
			for (unsigned int i=0;i<d.tamx;i++)
				sumdri+=d.Pixel(i,j)*r.Pixel(i,j);
		
		// optimizado: *TODO*
		// gracias a suponer que:
		// los rangos no tienen rotación y provienen de una referencia solamente
		// los dominios provienen de una referencia solamente
		// dominio/rango mínimo 2x2
		
		/*float* dpointer;
		float* rpointer;	
		short djump=d.xx+d.yx*d.ref->tamx;
		register unsigned int i,j;
		
		// C++

		// linea 0
		dpointer=d.ref->pixels+(d.posy)*d.ref->tamx+d.posx;
		rpointer=r.ref->pixels+(r.posy)*r.ref->tamx+r.posx;			
		for (i=0;i<d.tamx;i++)
		{
			sumdri+=(*dpointer)*(*rpointer);
			dpointer+=djump;
			rpointer++;
		}
		// linea 1
		dpointer=d.ref->pixels+(d.yy+d.posy)*d.ref->tamx+(d.xy+d.posx);
		rpointer=r.ref->pixels+(1+r.posy)*r.ref->tamx+r.posx;			
		for (i=0;i<d.tamx;i++)
		{
			sumdri+=(*dpointer)*(*rpointer);
			dpointer+=djump;
			rpointer++;
		}
		// demás lineas
		for (j=2;j<d.tamy;j++)
		{
			dpointer=d.ref->pixels+(j*d.yy+d.posy)*d.ref->tamx+(j*d.xy+d.posx);
			rpointer=r.ref->pixels+(j+r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i++)
			{
				sumdri+=(*dpointer)*(*rpointer);
				dpointer+=djump;
				rpointer++;
			}
		}*/

		// 3DNOW! intrinsics

		/*register __m64 res;
		res.m64_f32[0]=0.0f;
		res.m64_f32[1]=0.0f;
		_m_femms();
		if (djump==1)
		{
			// linea 0
			dpointer=d.ref->pixels+(d.posy)*d.ref->tamx+d.posx;
			rpointer=r.ref->pixels+(r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i+=2)
			{
				res=_m_pfadd(res,_m_pfmul(_m_from_floats(dpointer),_m_from_floats(rpointer)));
				dpointer+=2;
				rpointer+=2;
			}
			// linea 1
			dpointer=d.ref->pixels+(d.yy+d.posy)*d.ref->tamx+(d.xy+d.posx);
			rpointer=r.ref->pixels+(1+r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i+=2)
			{
				res=_m_pfadd(res,_m_pfmul(_m_from_floats(dpointer),_m_from_floats(rpointer)));
				dpointer+=2;
				rpointer+=2;
			}
			// demás lineas
			for (j=2;j<d.tamy;j++)
			{
				dpointer=d.ref->pixels+(j*d.yy+d.posy)*d.ref->tamx+(j*d.xy+d.posx);
				rpointer=r.ref->pixels+(j+r.posy)*r.ref->tamx+r.posx;			
				for (i=0;i<d.tamx;i+=2)
				{
					res=_m_pfadd(res,_m_pfmul(_m_from_floats(dpointer),_m_from_floats(rpointer)));
					dpointer+=2;
					rpointer+=2;
				}
			}
		}
		else
		{
			register __m64 aux;

			// linea 0
			dpointer=d.ref->pixels+(d.posy)*d.ref->tamx+d.posx;
			rpointer=r.ref->pixels+(r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i+=2)
			{
				aux.m64_f32[0]=*dpointer;
				dpointer+=djump;
				aux.m64_f32[1]=*dpointer;
				dpointer+=djump;
				res=_m_pfadd(res,_m_pfmul(aux,_m_from_floats(rpointer)));
				rpointer+=2;
			}
			// linea 1
			dpointer=d.ref->pixels+(d.yy+d.posy)*d.ref->tamx+(d.xy+d.posx);
			rpointer=r.ref->pixels+(1+r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i+=2)
			{
				aux.m64_f32[0]=*dpointer;
				dpointer+=djump;
				aux.m64_f32[1]=*dpointer;
				dpointer+=djump;
				res=_m_pfadd(res,_m_pfmul(aux,_m_from_floats(rpointer)));
				rpointer+=2;
			}
			// demás lineas
			for (j=2;j<d.tamy;j++)
			{
				dpointer=d.ref->pixels+(j*d.yy+d.posy)*d.ref->tamx+(j*d.xy+d.posx);
				rpointer=r.ref->pixels+(j+r.posy)*r.ref->tamx+r.posx;			
				for (i=0;i<d.tamx;i+=2)
				{
					aux.m64_f32[0]=*dpointer;
					dpointer+=djump;
					aux.m64_f32[1]=*dpointer;
					dpointer+=djump;
					res=_m_pfadd(res,_m_pfmul(aux,_m_from_floats(rpointer)));
					rpointer+=2;
				}
			}
		}
		res=_m_pfacc(res,res);
		sumdri=_m_to_float(res);*/

		// 3DNOW! assembler

		/*_asm
		{
			femms
			pxor mm0,mm0
		}
		__m64 aux;		
		if (djump==1)
		{
			for (unsigned int j=0;j<d.tamy;j++)
			{
				dpointer=&d.Pixel(0,j);			
				rpointer=&r.Pixel(0,j);
				for (unsigned int i=0;i<d.tamx;i+=2)
				{
					_asm
					{
						mov eax,dword ptr [rpointer] 
						movq mm1,mmword ptr [eax] 
						mov eax,dword ptr [dpointer] 
						movq mm2,mmword ptr [eax] 
						pfmul mm1,mm2
						pfadd mm0,mm1
					}
					dpointer+=2;
					rpointer+=2;
				}	
			}
			
		}
		else
		{
			for (unsigned int j=0;j<d.tamy;j++)
			{
				dpointer=&d.Pixel(0,j);			
				rpointer=&r.Pixel(0,j);
				for (unsigned int i=0;i<d.tamx;i+=2)
				{
					aux.m64_f32[0]=*dpointer;
					dpointer+=djump;
					aux.m64_f32[1]=*dpointer;
					dpointer+=djump;
					_asm
					{
						mov eax,dword ptr [rpointer] 
						movq mm1,mmword ptr [eax] 
						movq mm2,mmword ptr [aux] 
						pfmul mm1,mm2
						pfadd mm0,mm1
					}
					dpointer+=2;
					rpointer+=2;
				}	
			}
		}
		_asm
		{
			movd dword ptr [sumdri],mm0
			femms
		}*/

		// SSE

		/*static float buffer4float[4];
		if (djump==1)
		{
			if (d.tamx>2)
			{
				_asm
				{
					xorps xmm2,xmm2
				}
				for (unsigned int j=0;j<d.tamy;j++)
				{
					dpointer=&d.Pixel(0,j);			
					rpointer=&r.Pixel(0,j);
					for (unsigned int i=0;i<d.tamx;i+=4)
					{
						_asm
						{	
							mov edx,dpointer
							movups xmm0,[edx]
							mov edx,rpointer
							movups xmm1,[edx]
							mulps xmm0,xmm1
							addps xmm2,xmm0
						}
						dpointer+=4;
						rpointer+=4;
					}					
				}
				_asm
				{
					movups [buffer4float],xmm2
				}
				sumdri=buffer4float[0]+buffer4float[1]+buffer4float[2]+buffer4float[3];
			}
			else
			{
				dpointer=&d.Pixel(0,0);			
				rpointer=&r.Pixel(0,0);
				sumdri+=((*dpointer)*(*rpointer))+((*(dpointer+1))*(*(rpointer+1)));
				dpointer=&d.Pixel(0,1);			
				rpointer=&r.Pixel(0,1);
				sumdri+=((*dpointer)*(*rpointer))+((*(dpointer+1))*(*(rpointer+1)));
			}
		}
		else
		{
			if (d.tamx>2)
			{
				_asm
				{
					xorps xmm2,xmm2
				}
				for (unsigned int j=0;j<d.tamy;j++)
				{
					dpointer=&d.Pixel(0,j);			
					rpointer=&r.Pixel(0,j);
					for (unsigned int i=0;i<d.tamx;i+=4)
					{
						buffer4float[0]=*dpointer;
						dpointer+=djump;
						buffer4float[1]=*dpointer;
						dpointer+=djump;
						buffer4float[2]=*dpointer;
						dpointer+=djump;
						buffer4float[3]=*dpointer;
						dpointer+=djump;
						_asm
						{	
							movups xmm0,[buffer4float]
							mov edx,rpointer
							movups xmm1,[edx]
							mulps xmm0,xmm1
							addps xmm2,xmm0
						}
						rpointer+=4;
					}
				}
				_asm
				{
					movups [buffer4float],xmm2
				}
				sumdri=buffer4float[0]+buffer4float[1]+buffer4float[2]+buffer4float[3];
			}
			else
			{
				dpointer=&d.Pixel(0,0);			
				rpointer=&r.Pixel(0,0);
				sumdri+=((*dpointer)*(*rpointer))+((*(dpointer+djump))*(*(rpointer+1)));
				dpointer=&d.Pixel(0,1);			
				rpointer=&r.Pixel(0,1);
				sumdri+=((*dpointer)*(*rpointer))+((*(dpointer+djump))*(*(rpointer+1)));
			}
		}*/







		s2aux=(n*sumdri-d.sumi*r.sumi)/denom;

		// transformamos -1,0.93 en 0,31, -1,0.93 para asegurar contractividad
		//pis2aux=(int)(0.5f+(s2aux+1.0f)*16.0f);
		pis2aux=FloatToInt((s2aux+1.0f)*16.0f);
		if (pis2aux<0) pis2aux=0;
		else if (pis2aux>=32) pis2aux=31;
		s2aux=(float)pis2aux*0.0625f-1.0f;

		o2aux=(r.sumi-s2aux*d.sumi)*onedivn;	

		// 0,1 en 0,127
		// transformamos o2 de una manera extraña que utiliza el signo del s2
		faux1=(1.0f+fabs(s2aux))*0.007874015748031f;
		if (s2aux>0.0f) 
			o2aux+=s2aux;
		pio2aux=FloatToInt(o2aux/faux1);
		if (pio2aux<0) pio2aux=0;
		if (pio2aux>=128) pio2aux=127;
		// recalculamos o2 con nuevo valor discreto
		o2aux=(float)pio2aux*faux1;
		if (s2aux>0.0f) 
			o2aux-=s2aux;

		s2=s2aux;
		o2=o2aux;
		pis2=pis2aux;
		pio2=pio2aux;

		return /*sqrtf*/fabs(r.sumsqri+s2aux*(s2aux*d.sumsqri-2.0f*sumdri+2.0f*o2aux*d.sumi)+o2aux*(o2aux*n-2.0f*r.sumi))*onedivn;
	}
}

void FLDomains2::OptimizeTrans(FLTrans2* ret,FLBitmapRef2& range)
{
	float optimrms=10e10f;
	float rms2,s2,o2;
	unsigned int cont;
	int qcont=-1;
	int rangeclass=range.SolveSums();
	int clacont=rangeclass;
	int i;
	int pis2,pio2;

	vector<FLBitmapRef2*>::iterator it=domain[rangeclass].begin();
	vector<FLBitmapRef2*>::iterator itend=domain[rangeclass].end();
	for (cont=0;it!=itend;it++,cont++)
	{
		rms2=LeastSquares2(*(*it),range,s2,o2,pis2,pio2);
		if (rms2<optimrms)
		{
			optimrms=rms2;
			qcont=cont;
		}
	}
	
	if (qcont==-1)
	{
		for (i=0;i<24;i++)
		{
			if (i!=rangeclass)
			{
				for (it=domain[i].begin(),cont=0;it!=domain[i].end();it++,cont++)
				{
					rms2=LeastSquares2(*(*it),range,s2,o2,pis2,pio2);
					if (rms2<optimrms)
					{
						optimrms=rms2;
						qcont=cont;
						clacont=i;
					}
				}
			}
		}
	}

	LeastSquares2(*(domain[clacont][qcont]),range,s2,o2,pis2,pio2);

	ret->domposx=domain[clacont][qcont]->posglobalx;
	ret->domposy=domain[clacont][qcont]->posglobaly;
	ret->ranposx=range.posglobalx;
	ret->ranposy=range.posglobaly;
	if (domsize==2)
	{
		ret->domsize=2;
		ret->ransize=1;
	}
	else
	{
		ret->domsize=range.tamx<<1;
		ret->ransize=range.tamx;
	}
	ret->ori=domain[clacont][qcont]->ori;
	ret->s=s2;
	ret->o=o2;
	ret->pis=pis2;
	ret->pio=pio2;
	ret->rms=optimrms;

	/*static float mins=999.0f;
	static float maxs=-999.0f;
	static float mino=999.0f;
	static float maxo=-999.0f;
	if (s2<mins) mins=s2;
	if (s2>maxs) maxs=s2;
	if (o2<mino) mino=o2;
	if (o2>maxo) maxo=o2;*/
}

bool FLDomains3::AddIfDifferent(FLBitmapRef3* domainaux)
{
	static unsigned int sumsaux;
	static vector<FLBitmapRef3*>::iterator it;
	static vector<FLBitmapRef3*>::iterator itend;
	static map<float,vector<FLBitmapRef3*> >::iterator itprimerpixel;
	static float pixelvalue;

	sumsaux=domainaux->SolveSums();
	pixelvalue=domainaux->Pixel(0,0,0);
	itprimerpixel=domainorder.find(pixelvalue);
	if (itprimerpixel==domainorder.end())
	{
		vector<FLBitmapRef3*> auxvector;
		auxvector.push_back(domainaux);
		domainorder.insert(pair<float,vector<FLBitmapRef3*> >(pixelvalue,auxvector));
		domain[sumsaux].push_back(domainaux);
		return true;
	}
	it=itprimerpixel->second.begin();
	itend=itprimerpixel->second.end();
	for (;it!=itend;it++)
	{
		if ((**it)==(*domainaux))
		{
			delete domainaux;
			return false;
		}
	}
	itprimerpixel->second.push_back(domainaux);
	domain[sumsaux].push_back(domainaux);
	return true;
}

FLDomains3::FLDomains3(FLBitmap3* bitmap,unsigned short domsize2,unsigned short domainsalto)
{
	domsize=domsize2;
	domsalto=domainsalto;
	
	unsigned int numdoms=0;

	unsigned short ransize=domsize>>1;

	if (ransize==1)
	{
		bitmap2x2[0]=new FLBitmap3(*bitmap);
		bitmap2x2[1]=bitmap2x2[2]=bitmap2x2[3]=bitmap2x2[4]=bitmap2x2[5]=bitmap2x2[6]=bitmap2x2[7]=NULL;

		// cojo los dominios de 2 sin subsampling

		FLBitmapRef3 *domainaux;

		unsigned int domainstamx=(bitmap2x2[0]->tamx-domsize)/domainsalto+1;
		unsigned int domainstamy=(bitmap2x2[0]->tamy-domsize)/domainsalto+1;
		unsigned int domainstamz=(bitmap2x2[0]->tamz-domsize)/domainsalto+1;
		unsigned int domainstam=domainstamx*domainstamy*domainstamz*32;

		cout << "solving domains of " << domsize << " [" << domainstam << "] ... ";
		cout.flush();

		unsigned short i2,j2,k2;
		unsigned short i2fin,j2fin,k2fin;
		k2fin=bitmap2x2[0]->tamz-domsize;
		j2fin=bitmap2x2[0]->tamy-domsize;
		i2fin=bitmap2x2[0]->tamx-domsize;
		unsigned short desp=domsize-1;
		for (k2=0;k2<=k2fin;k2+=domainsalto)
		{
			for (j2=0;j2<=j2fin;j2+=domainsalto)
			{
				for (i2=0;i2<=i2fin;i2+=domainsalto)
				{
					// truco: quién es el que queda en 0,0
					// flipear y rotar

					// si el dominio sin rotación ya existe descartamos 
					// aún podrían darse dominios repetidos si los originales son rotaciones de alguno dado

					// orig y los rotates 90º x,y,z
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2,					domsize,domsize,domsize,bitmap2x2[0]);
					if (!AddIfDifferent(domainaux))	continue;
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2+desp,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(1);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(2);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(3);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en x
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(4);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2+desp,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(5);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2,					domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(6);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(7);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en y
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(8);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(9);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(10);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2,					domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(11);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en z
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2+desp,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(12);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2,					domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(13);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(14);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(15);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xy
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(16);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2+desp,	domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(17);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(18);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(19);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(20);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(21);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2+desp,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(22);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2+desp,	domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(23);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en yz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(24);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(25);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2+desp,	domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(26);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2,k2+desp,				domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(27);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xyz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2+desp,	domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(28);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2+desp,k2,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(29);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2,j2+desp,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(30);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2+desp,j2,k2+desp,			domsize,domsize,domsize,bitmap2x2[0]);
					domainaux->SetOrientation(31);
					domain[domainaux->SolveSums()].push_back(domainaux);

					numdoms+=32;
				}
			}
		}
		cout << numdoms << endl;
	}
	else
	{
		bitmap2x2[0]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
		bitmap->SubSample2(*bitmap2x2[0]);
		if ((domainsalto%2)!=0)
		{
			FLBitmap3 bitmap2x2aux(bitmap->tamx,bitmap->tamy,bitmap->tamz);
			bitmap2x2[1]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,1,0,0,bitmap->tamx-1,bitmap->tamy,bitmap->tamz);
			bitmap2x2aux.SubSample2(*bitmap2x2[1]);
			bitmap2x2[2]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,0,1,0,bitmap->tamx,bitmap->tamy-1,bitmap->tamz);
			bitmap2x2aux.SubSample2(*bitmap2x2[2]);
			bitmap2x2[3]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,1,1,0,bitmap->tamx-1,bitmap->tamy-1,bitmap->tamz);
			bitmap2x2aux.SubSample2(*bitmap2x2[3]);
			bitmap2x2[4]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,0,0,1,bitmap->tamx,bitmap->tamy,bitmap->tamz-1);
			bitmap2x2aux.SubSample2(*bitmap2x2[4]);
			bitmap2x2[5]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,1,0,1,bitmap->tamx-1,bitmap->tamy,bitmap->tamz-1);
			bitmap2x2aux.SubSample2(*bitmap2x2[5]);
			bitmap2x2[6]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,0,1,1,bitmap->tamx,bitmap->tamy-1,bitmap->tamz-1);
			bitmap2x2aux.SubSample2(*bitmap2x2[6]);
			bitmap2x2[7]=new FLBitmap3(bitmap->tamx>>1,bitmap->tamy>>1,bitmap->tamz>>1);
			bitmap->SubImage(bitmap2x2aux,1,1,1,bitmap->tamx-1,bitmap->tamy-1,bitmap->tamz-1);
			bitmap2x2aux.SubSample2(*bitmap2x2[7]);
		}

		FLBitmapRef3 *domainaux;

		unsigned int domainstamx=(bitmap->tamx-domsize)/domainsalto+1;
		unsigned int domainstamy=(bitmap->tamy-domsize)/domainsalto+1;
		unsigned int domainstamz=(bitmap->tamz-domsize)/domainsalto+1;
		unsigned int domainstam=domainstamx*domainstamy*domainstamz*32;

		cout << "solving domains of " << domsize << " [" << domainstam << "] ... ";
		cout.flush();

		unsigned short i2,j2,k2;
		unsigned short i2fin,j2fin,k2fin;
		k2fin=bitmap->tamz-domsize;
		j2fin=bitmap->tamy-domsize;
		i2fin=bitmap->tamx-domsize;
		unsigned short i2div2,j2div2,k2div2;
		unsigned short desp=ransize-1;
		FLBitmap3* bitmapaux;
		for (k2=0;k2<=k2fin;k2+=domainsalto)
		{
			k2div2=k2>>1;
			for (j2=0;j2<=j2fin;j2+=domainsalto)
			{
				j2div2=j2>>1;
				for (i2=0;i2<=i2fin;i2+=domainsalto)
				{
					i2div2=i2>>1;
					bitmapaux=bitmap2x2[((k2&1)<<2)|((j2&1)<<1)|(i2&1)];

					// truco: quién es el que queda en 0,0
					// flipear y rotar

					// si el dominio sin rotación ya existe descartamos 
					// aún podrían darse dominios repetidos si los originales son rotaciones de alguno dado

					// orig y los rotates 90º x,y,z
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2,					ransize,ransize,ransize,bitmapaux);
					if (!AddIfDifferent(domainaux))	continue;
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2+desp,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(1);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(2);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(3);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en x
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(4);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2+desp,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(5);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2,					ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(6);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(7);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en y
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(8);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(9);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(10);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2,					ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(11);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en z
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2+desp,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(12);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2,					ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(13);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(14);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(15);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xy
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(16);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2+desp,	ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(17);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(18);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(19);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(20);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(21);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2+desp,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(22);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2+desp,	ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(23);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en yz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(24);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(25);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2+desp,	ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(26);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2,k2div2+desp,				ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(27);
					domain[domainaux->SolveSums()].push_back(domainaux);
					// los flips en xyz
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2+desp,	ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(28);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2+desp,k2div2,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(29);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2,j2div2+desp,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(30);
					domain[domainaux->SolveSums()].push_back(domainaux);
					domainaux=new FLBitmapRef3(i2,j2,k2,	i2div2+desp,j2div2,k2div2+desp,			ransize,ransize,ransize,bitmapaux);
					domainaux->SetOrientation(31);
					domain[domainaux->SolveSums()].push_back(domainaux);

					numdoms+=32;
				}
			}
		}
		cout << numdoms << endl;
	}
}

FLDomains3::~FLDomains3(void)
{
	int i;

	for (i=0;i<(((domsalto%2)!=0)?8:1);i++)
		if (bitmap2x2[i]) delete bitmap2x2[i];

	vector<FLBitmapRef3*>::iterator it;
	for (i=0;i<40320;i++)
	{
		for (it=domain[i].begin();it!=domain[i].end();it++)
			delete *it;
		domain[i].clear();
	}
	map<float,vector<FLBitmapRef3*> >::iterator it2;
	for (it2=domainorder.begin();it2!=domainorder.end();it2++)
		(*it2).second.clear();
}

float LeastSquares3(const FLBitmapRef3& d, const FLBitmapRef3& r, float& s2, float& o2, int& pis2, int& pio2)
{
	float n=(float)d.numpixels;
	
	//float onedivn=1.0f/n;
	float onedivn=FloatInverse(n);
	
	float denom=n*d.sumsqri-d.sumi*d.sumi;
		
	if (fabs(denom)<0.000001)
	{
		float o2aux;
		int pio2aux;

		pis2=16;
		s2=0.0f;

		o2aux=r.sumi*onedivn;
		
		// calculamos pio2
		// se transforma 0,1 en 0,127
		pio2aux=FloatToInt(o2aux*127.0f);
		
		if (pio2aux<0) pio2aux=0;
		else if (pio2aux>=128) pio2aux=127;
		
		// recalculamos o2 con nuevo valor discreto
		o2aux=(float)pio2aux*0.007874015748031f;

		o2=o2aux;
		pio2=pio2aux;

		return /*sqrtf*/fabs(r.sumsqri+o2aux*(o2aux*n-2.0f*r.sumi))*onedivn;
	}
	else
	{
		float s2aux,o2aux;
		float faux1;
		int pis2aux,pio2aux;
		
		register float sumdri=0.0f;
		

		// sin optimizar
		/*for (unsigned int j=0;j<d.tamy;j++)
			for (unsigned int i=0;i<d.tamx;i++)
				sumdri+=d.Pixel(i,j)*r.Pixel(i,j);*/
		
		// optimizado: *TODO*
		// gracias a suponer que:
		// los rangos no tienen rotación y provienen de una referencia solamente
		// los dominios provienen de una referencia solamente
		// dominio/rango mínimo 2x2
		
		//float* dpointer;
		//float* rpointer;	
		//short djump=d.xx+d.yx*d.ref->tamx;
		register unsigned int i,j,k;

		// C++

		for (k=0;k<d.tamz;k++)
		{
			for (j=0;j<d.tamy;j++)
			{
				for (i=0;i<d.tamx;i++)
				{
					sumdri+=d.Pixel(i,j,k)*r.Pixel(i,j,k);
				}
			}
		}

		/*// linea 0
		dpointer=d.ref->pixels+(d.posy)*d.ref->tamx+d.posx;
		rpointer=r.ref->pixels+(r.posy)*r.ref->tamx+r.posx;			
		for (i=0;i<d.tamx;i++)
		{
			sumdri+=(*dpointer)*(*rpointer);
			dpointer+=djump;
			rpointer++;
		}
		// linea 1
		dpointer=d.ref->pixels+(d.yy+d.posy)*d.ref->tamx+(d.xy+d.posx);
		rpointer=r.ref->pixels+(1+r.posy)*r.ref->tamx+r.posx;			
		for (i=0;i<d.tamx;i++)
		{
			sumdri+=(*dpointer)*(*rpointer);
			dpointer+=djump;
			rpointer++;
		}
		// demás lineas
		for (j=2;j<d.tamy;j++)
		{
			dpointer=d.ref->pixels+(j*d.yy+d.posy)*d.ref->tamx+(j*d.xy+d.posx);
			rpointer=r.ref->pixels+(j+r.posy)*r.ref->tamx+r.posx;			
			for (i=0;i<d.tamx;i++)
			{
				sumdri+=(*dpointer)*(*rpointer);
				dpointer+=djump;
				rpointer++;
			}
		}*/

		s2aux=(n*sumdri-d.sumi*r.sumi)/denom;

		// transformamos -1,0.93 en 0,31, -1,0.93 para asegurar contractividad
		//pis2aux=(int)(0.5f+(s2aux+1.0f)*16.0f);
		pis2aux=FloatToInt((s2aux+1.0f)*16.0f);
		if (pis2aux<0) pis2aux=0;
		else if (pis2aux>=32) pis2aux=31;
		s2aux=(float)pis2aux*0.0625f-1.0f;

		o2aux=(r.sumi-s2aux*d.sumi)*onedivn;	

		// 0,1 en 0,127
		// transformamos o2 de una manera extraña que utiliza el signo del s2
		faux1=(1.0f+fabs(s2aux))*0.007874015748031f;
		if (s2aux>0.0f) 
			o2aux+=s2aux;
		pio2aux=FloatToInt(o2aux/faux1);
		if (pio2aux<0) pio2aux=0;
		if (pio2aux>=128) pio2aux=127;
		// recalculamos o2 con nuevo valor discreto
		o2aux=(float)pio2aux*faux1;
		if (s2aux>0.0f) 
			o2aux-=s2aux;

		s2=s2aux;
		o2=o2aux;
		pis2=pis2aux;
		pio2=pio2aux;

		return /*sqrtf*/fabs(r.sumsqri+s2aux*(s2aux*d.sumsqri-2.0f*sumdri+2.0f*o2aux*d.sumi)+o2aux*(o2aux*n-2.0f*r.sumi))*onedivn;
	}
}

void FLDomains3::OptimizeTrans(FLTrans3* ret,FLBitmapRef3& range)
{
	float optimrms=10e10f;
	float rms2,s2,o2;
	unsigned int cont;
	int qcont=-1;
	int rangeclass=range.SolveSums();
	int clacont=rangeclass;
	int i;
	int pis2,pio2;

	vector<FLBitmapRef3*>::iterator it=domain[rangeclass].begin();
	vector<FLBitmapRef3*>::iterator itend=domain[rangeclass].end();
	for (cont=0;it!=itend;it++,cont++)
	{
		rms2=LeastSquares3(*(*it),range,s2,o2,pis2,pio2);
		if (rms2<optimrms)
		{
			optimrms=rms2;
			qcont=cont;
		}
	}

	if (qcont==-1)
	{
		for (i=0;i<40320;i++)
		{
			if (i!=rangeclass)
			{
				for (it=domain[i].begin(),cont=0;it!=domain[i].end();it++,cont++)
				{
					rms2=LeastSquares3(*(*it),range,s2,o2,pis2,pio2);
					if (rms2<optimrms)
					{
						optimrms=rms2;
						qcont=cont;
						clacont=i;
					}
				}
			}
		}
	}

	LeastSquares3(*(domain[clacont][qcont]),range,s2,o2,pis2,pio2);

	ret->domposx=domain[clacont][qcont]->posglobalx;
	ret->domposy=domain[clacont][qcont]->posglobaly;
	ret->domposz=domain[clacont][qcont]->posglobalz;
	ret->ranposx=range.posglobalx;
	ret->ranposy=range.posglobaly;
	ret->ranposz=range.posglobalz;
	if (domsize==2)
	{
		ret->domsize=2;
		ret->ransize=1;
	}
	else
	{
		ret->domsize=range.tamx<<1;
		ret->ransize=range.tamx;
	}
	ret->ori=domain[clacont][qcont]->ori;
	ret->s=s2;
	ret->o=o2;
	ret->pis=pis2;
	ret->pio=pio2;
	ret->rms=optimrms;
}