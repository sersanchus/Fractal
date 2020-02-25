#include "FLBitmap.h"
#include <math.h>
#include <string>

FLBitmap2::FLBitmap2(void)
{
	posglobalx=0;
	posglobaly=0;
/*	xx=1;
	xy=0;
	yx=0;
	yy=1;
	ori=0;*/
	tamx=0;
	tamy=0;
	numpixels=0;
	pixels=new float[numpixels];
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap2::FLBitmap2(const FLBitmap2 &b)
{
	posglobalx=0;
	posglobaly=0;
	tamx=b.tamx;
	tamy=b.tamy;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	memcpy(pixels,b.pixels,numpixels*sizeof(float)); 
	/*xx=b.xx;
	xy=b.xy;
	yx=b.yx;
	yy=b.yy;
	ori=b.ori;*/
	/*sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
}

FLBitmap2::FLBitmap2(unsigned short tam2)
{
	posglobalx=0;
	posglobaly=0;
	/*xx=1;
	xy=0;
	yx=0;
	yy=1;
	ori=0;*/
	tamx=tam2;
	tamy=tam2;
	numpixels=tam2*tam2;
	pixels=new float[numpixels];
	/*for (unsigned int i=0;i<numpixels;i++)
		pixels[i]=0.0f;*/
	memset(pixels,0,numpixels*sizeof(float));
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap2::FLBitmap2(unsigned short tamx2,unsigned short tamy2)
{
	posglobalx=0;
	posglobaly=0;
	/*xx=1;
	xy=0;
	yx=0;
	yy=1;
	ori=0;*/
	tamx=tamx2;
	tamy=tamy2;
	numpixels=tamx*tamy;
	pixels=new float[numpixels];
	/*for (unsigned int i=0;i<numpixels;i++)
		pixels[i]=0.0f;*/
	memset(pixels,0,numpixels*sizeof(float));
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap2::~FLBitmap2()
{
	delete[] pixels;
}

FLBitmap2& FLBitmap2::operator=(const FLBitmap2 &b)
{
	delete[] pixels;
	
	tamx=b.tamx;
	tamy=b.tamy;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	memcpy(pixels,b.pixels,numpixels*sizeof(float)); 
	/*xx=b.xx;
	xy=b.xy;
	yx=b.yx;
	yy=b.yy;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
	return (FLBitmap2&)b;
}

FLBitmap2& FLBitmap2::operator=(const FLBitmapRef2 &b)
{
	delete[] pixels;
	
	tamx=b.tamx;
	tamy=b.tamy;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	float *pointer=pixels;
	for (unsigned short j=0;j<tamy;j++)
	{
		for (unsigned short i=0;i<tamx;i++)
		{
			*pointer=b.Pixel(i,j);
			pointer++;
		}
	}
	/*xx=b.xx;
	xy=b.xy;
	yx=b.yx;
	yy=b.yy;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
	return *this;
}

bool FLBitmap2::operator==(const FLBitmap2 &b)
{
	float *aux1=pixels;
	float *aux2=b.pixels;
	for (unsigned int i=0;i<numpixels;i++)
	{
		if (*aux1!=*aux2)
			return false;
		aux1++;
		aux2++;
	}
	return true;
}

bool FLBitmap2::operator==(const FLBitmapRef2 &b)
{
	float *aux1=pixels;
	for (unsigned int j=0;j<tamy;j++)
	{
		for (unsigned int i=0;i<tamx;i++)
		{
			if (*aux1!=b.Pixel(i,j))
				return false;
			aux1++;
		}
	}
	return true;
}

void FLBitmap2::SubSample2(FLBitmap2& ret) const
{
	unsigned int bucleaux1,bucleaux2,bucleaux3;
	bucleaux1=bucleaux2=0;
	bucleaux3=tamx;
	for (unsigned short j=0;j<ret.tamy;j++)
	{
		for (unsigned short i=0;i<ret.tamx;i++)
		{
			ret.pixels[bucleaux1]=
				(
					pixels[bucleaux2]+
					pixels[bucleaux2+1]+
					pixels[bucleaux3]+
					pixels[bucleaux3+1]
				)*0.25f;
			bucleaux1++;
			bucleaux2+=2;
			bucleaux3+=2;
		}
		bucleaux2+=tamx;
		bucleaux3+=tamx;
	}	
}

void FLBitmap2::SuperSample2(unsigned short i,unsigned short j,FLBitmap2& ret) const
{
	unsigned int aux;
	unsigned int contx=0;

	for (int j2=j-1;j2<j+1;j2++)
	{
		for (int i2=i-1;i2<i+1;i2++)
		{
			aux=0;
			ret.pixels[contx]=0.0f;
			for (int j3=j2;j3<j2+2;j3++)
			{
				for (int i3=i2;i3<i2+2;i3++)
				{
					if (i3>=0 && i3<tamx && j3>=0 && j3<tamy)
					{
						aux++;
						ret.pixels[contx]+=Pixel(i3,j3);
					}
				}
			}
			ret.pixels[contx]/=(float)aux;
			contx++;
		}					
	}

	/*float alpha=Pixel(i,j)/((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3])*0.25f);
	for (unsigned int aux=0;aux<4;aux++)
		ret.pixels[aux]*=alpha;*/

	float dif=Pixel(i,j)-((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3])*0.25f);
	bool yaesta[4]={false,false,false,false};
	unsigned int yaestan=0;
	unsigned int cual;

	if (dif>=0.0f)
	{
		while(yaestan<4)
		{
			float mayor=0.0f;
			cual=0;
			for (unsigned int ir=0;ir<4;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]>=mayor)
				{
					mayor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)>1.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif-1.0f)/((float)(4-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=1.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<4;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	else
	{
		while(yaestan<4)
		{
			float menor=999999.9f;
			cual=0;
			for (unsigned int ir=0;ir<4;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]<=menor)
				{
					menor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)<0.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif)/((float)(4-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=0.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<4;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	
	for (aux=0;aux<4;aux++)
	{
		if (ret.pixels[aux]<0.0f) ret.pixels[aux]=0.0f;
		if (ret.pixels[aux]>1.0f) ret.pixels[aux]=1.0f;
	}	
}

void FLBitmap2::SubImage(FLBitmap2& ret,unsigned short x,unsigned short y,unsigned short auxtamx,unsigned short auxtamy) const
{
	float *orig=pixels+y*tamx+x;
	float *dest=ret.pixels;
	for (unsigned short j=0;j<auxtamy;j++)
	{
		memcpy(dest,orig,auxtamx*sizeof(float));
		orig+=tamx;
		dest+=ret.tamx;
	}
}

void FLBitmap2::SubImage(FLBitmapRef2& ret,unsigned short x,unsigned short y,unsigned short auxtamx,unsigned short auxtamy) const
{
	ret.posx=x;
	ret.posy=y;
	ret.tamx=auxtamx;
	ret.tamy=auxtamy;
	ret.numpixels=auxtamx*auxtamy;
	ret.ref=(FLBitmap2*)this;
	/*ret.xx=xx;
	ret.xy=xy;
	ret.yx=yx;
	ret.yy=yy;
	ret.ori=ori;*/
}

/*void FLBitmap2::SetOrientation(unsigned char ori2)
{
	//  0      1      2      3      4      5      6      7
	// orig   90º    180º   270º   flipx   90º   180º   270º
	// |1 0| |0  1| |-1 0| |0 -1| |-1 0| |0 -1| |1 0 | |0 1| 
	// |0 1| |-1 0| |0 -1| |1  0| |0 -1| |-1 0| |0 -1| |1 0| 
	
	ori=ori2;
	switch(ori)
	{
		case 0:
			xx=yy=1;
			xy=yx=0;
			break;
		case 1:
			xx=yy=0;
			xy=1;
			yx=-1;
			break;
		case 2:
			xx=yy=-1;
			xy=yx=0;
			break;
		case 3:
			xx=yy=0;
			xy=-1;
			yx=1;
			break;
		case 4:
			xx=-1;
			yy=1;
			xy=yx=0;
			break;
		case 5:
			xx=yy=0;
			xy=yx=-1;
			break;
		case 6:
			xx=1;
			yy=-1;
			xy=yx=0;
			break;
		case 7:
			xx=yy=0;
			xy=yx=1;
			break;
	}
}*/

/*unsigned int FLBitmap2::SolveSums(void)
{
	float faux=0.0f;
	float sumaux1=0.0f;
	float sumaux2=0.0f;
	float sumaux3=0.0f;
	float sumaux4=0.0f;
	float sumsqraux1=0.0f;
	float sumsqraux2=0.0f;
	float sumsqraux3=0.0f;
	float sumsqraux4=0.0f;
	unsigned short tamxdiv2=tamx>>1;
	unsigned short tamydiv2=tamy>>1;
	unsigned int bucleaux;
	register unsigned short i,j;

	// square1
	bucleaux=0;
	for (j=0;j<tamydiv2;j++)
	{
		for (i=0;i<tamxdiv2;i++) 
		{	
			faux=pixels[bucleaux];
			sumaux1+=faux;
			sumsqraux1+=faux*faux;
			bucleaux++;
		}
		bucleaux+=tamx;
	}
	
	// square2
	bucleaux=tamxdiv2;
	for (j=0;j<tamydiv2;j++)
	{
		for (i=tamxdiv2;i<tamx;i++) 
		{	
			faux=pixels[bucleaux];
			sumaux2+=faux;
			sumsqraux2+=faux*faux;
			bucleaux++;
		}
		bucleaux+=tamx;
	}
	
	// square3
	bucleaux=tamydiv2*tamx;
	for (j=tamydiv2;j<tamy;j++)
	{
		for (i=0;i<tamxdiv2;i++) 
		{	
			faux=pixels[bucleaux];
			sumaux3+=faux;
			sumsqraux3+=faux*faux;
			bucleaux++;
		}
		bucleaux+=tamx;
	}
	
	// square4
	bucleaux=tamydiv2*tamx+tamxdiv2;
	for (j=tamydiv2;j<tamy;j++)
	{
		for (i=tamxdiv2;i<tamx;i++) 
		{	
			faux=pixels[bucleaux];
			sumaux4+=faux;
			sumsqraux4+=faux*faux;
			bucleaux++;
		}
		bucleaux+=tamx;
	}
	
	// sumi, sumsqri
	sumi=sumaux1+sumaux2+sumaux3+sumaux4;
	sumsqri=sumsqraux1+sumsqraux2+sumsqraux3+sumsqraux4;

	// classify
	unsigned int classify=0;
	classify|=((sumaux1>=sumaux2)+(sumaux1>=sumaux3)+(sumaux1>=sumaux4))<<6;
	classify|=((sumaux2>=sumaux1)+(sumaux2>=sumaux3)+(sumaux2>=sumaux4))<<4;
	classify|=((sumaux3>=sumaux1)+(sumaux3>=sumaux2)+(sumaux3>=sumaux4))<<2;
	classify|=((sumaux4>=sumaux1)+(sumaux4>=sumaux2)+(sumaux4>=sumaux3));
	return classify;
}*/

float FLBitmap2::DiferenceMean(const FLBitmap2& bit) const
{
	float dif=0.0f;
	for (unsigned int i=0;i<numpixels;i++)
		dif+=fabs(bit.pixels[i]-pixels[i]);
	return dif/(float)numpixels;
}

float FLBitmap2::DiferenceMax(const FLBitmap2& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned int i=0;i<numpixels;i++)
	{
		aux=fabs(bit.pixels[i]-pixels[i]);
		if (aux>dif) dif=aux;
	}
	return dif;
}

float FLBitmap2::DiferenceMse(const FLBitmap2& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned int i=0;i<numpixels;i++)
	{
		aux=bit.pixels[i]-pixels[i];
		dif+=aux*aux;
	}
	return dif/(float)numpixels;
}

float FLBitmap2::DiferencePsnr(const FLBitmap2& bit) const
{
	/*
	  http://pub.ufasta.edu.ar/SISD/jpeg/psnr.htm
	  El rango de los valores PSNR típicos son entre 20 y 40. 
	  Se utilizan generalmente dos decimales(Ej, 25.47). 
	  El valor real no es significativo, pero la comparación entre dos valores para diversas imágenes reconstruidas da una medida de calidad.
	  El comité del MPEG utilizó un umbral informal de 0.5 DB PSNR para decidir si incorporar una optimización de la codificación porque creyó que una mejora de esa magnitud sería visible.
	*/

	// dif es MSE - Error Cuadrático Medio
	float dif=DiferenceMse(bit);
	
	// dif es RMSE
	dif=sqrtf(dif); 
	
	// calculo el PSNR (db) peak signal-to-noise ratio
	//return 20.0f*logf(1.0f/dif)*0.43429448190325f;
	return 20.0f*log10f(1.0f/dif);
}

void FLBitmap2::Clear(void)
{
	/*float* pointer=pixels;
	for (unsigned int i=0;i<numpixels;i++)
	{
		*pointer=1.0f;
		pointer++;
	}*/
	memset(pixels,0,numpixels*sizeof(float));
}

FLBitmap3::FLBitmap3(void)
{
	posglobalx=posglobaly=posglobalz=0;
	/*xx=1;xy=0;xz=0;yx=0;yy=1;yz=0;zx=0;zy=0;zz=1;
	ori=0;*/
	tamx=tamy=tamz=0;
	numpixels=0;
	pixels=new float[numpixels];
	tamxpertamy=0;
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap3::FLBitmap3(const FLBitmap3 &b)
{
	posglobalx=posglobaly=posglobalz=0;
	tamx=b.tamx;
	tamy=b.tamy;
	tamz=b.tamz;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	memcpy(pixels,b.pixels,numpixels*sizeof(float)); 
	tamxpertamy=b.tamxpertamy;
	/*xx=b.xx;
	xy=b.xy;
	xz=b.xz;
	yx=b.yx;
	yy=b.yy;
	yz=b.yz;
	zx=b.zx;
	zy=b.zy;
	zz=b.zz;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
}

FLBitmap3::FLBitmap3(unsigned short tam2)
{
	posglobalx=posglobaly=posglobalz=0;
	/*xx=1;xy=0;xz=0;yx=0;yy=1;yz=0;zx=0;zy=0;zz=1;
	ori=0;*/
	tamx=tamy=tamz=tam2;
	numpixels=tamx*tamy*tamz;
	pixels=new float[numpixels];
	/*for (unsigned int i=0;i<numpixels;i++)
		pixels[i]=255.0f;*/
	memset(pixels,0,numpixels*sizeof(float));
	tamxpertamy=tamx*tamy;
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap3::FLBitmap3(unsigned short tamx2,unsigned short tamy2,unsigned short tamz2)
{
	posglobalx=posglobaly=posglobalz=0;
	/*xx=1;xy=0;xz=0;yx=0;yy=1;yz=0;zx=0;zy=0;zz=1;
	ori=0;*/
	tamx=tamx2;
	tamy=tamy2;
	tamz=tamz2;
	numpixels=tamx*tamy*tamz;
	pixels=new float[numpixels];
	/*for (unsigned int i=0;i<numpixels;i++)
		pixels[i]=255.0f;*/
	memset(pixels,0,numpixels*sizeof(float));
	tamxpertamy=tamx*tamy;
	//sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmap3::~FLBitmap3()
{
	delete[] pixels;
}

FLBitmap3& FLBitmap3::operator=(const FLBitmap3 &b)
{
	delete[] pixels;
	
	tamx=b.tamx;
	tamy=b.tamy;
	tamz=b.tamz;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	memcpy(pixels,b.pixels,numpixels*sizeof(float)); 
	/*xx=b.xx;
	xy=b.xy;
	xz=b.xz;
	yx=b.yx;
	yy=b.yy;
	yz=b.yz;
	zx=b.zx;
	zy=b.zy;
	zz=b.zz;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
	tamxpertamy=b.tamxpertamy;
	return (FLBitmap3&)b;
}

FLBitmap3& FLBitmap3::operator=(const FLBitmapRef3 &b)
{
	delete[] pixels;
	
	tamx=b.tamx;
	tamy=b.tamy;
	tamz=b.tamz;
	numpixels=b.numpixels;
	pixels=new float[numpixels];
	float *pointer=pixels;
	for (unsigned short k=0;k<tamz;k++)
	{
		for (unsigned short j=0;j<tamy;j++)
		{
			for (unsigned short i=0;i<tamx;i++)
			{
				*pointer=b.Pixel(i,j,k);
				pointer++;
			}
		}
	}
	tamxpertamy=b.tamxpertamy;
	/*xx=b.xx;
	xy=b.xy;
	xz=b.xz;
	yx=b.yx;
	yy=b.yy;
	yz=b.yz;
	zx=b.zx;
	zy=b.zy;
	zz=b.zz;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;*/
	//classify=b.classify;
	return *this;
}

bool FLBitmap3::operator==(const FLBitmap3 &b)
{
	float *aux1=pixels;
	float *aux2=b.pixels;
	for (unsigned int i=0;i<numpixels;i++)
	{
		if (*aux1!=*aux2)
			return false;
		aux1++;
		aux2++;
	}
	return true;
}

bool FLBitmap3::operator==(const FLBitmapRef3 &b)
{
	float *aux1=pixels;
	for (unsigned int k=0;k<tamz;k++)
	{
		for (unsigned int j=0;j<tamy;j++)
		{
			for (unsigned int i=0;i<tamx;i++)
			{
				if (*aux1!=b.Pixel(i,j,k))
					return false;
				aux1++;
			}
		}
	}
	return true;
}

void FLBitmap3::SubSample2(FLBitmap3& ret) const
{
	unsigned int posfinal,posaux1,posaux2,posaux3,posaux4,aux;
	posfinal=posaux1=0;
	posaux2=tamx;
	aux=tamx*tamy;
	posaux3=aux;
	posaux4=aux+tamx;
	for (unsigned short k=0;k<ret.tamz;k++)
	{
		for (unsigned short j=0;j<ret.tamy;j++)
		{
			for (unsigned short i=0;i<ret.tamx;i++)
			{
				ret.pixels[posfinal]=
					(
						pixels[posaux1]+
						pixels[posaux1+1]+
						pixels[posaux2]+
						pixels[posaux2+1]+
						pixels[posaux3]+
						pixels[posaux3+1]+
						pixels[posaux4]+
						pixels[posaux4+1]
					)*0.125f;
				posfinal++;
				posaux1+=2;
				posaux2+=2;
				posaux3+=2;
				posaux4+=2;
			}
			posaux1+=tamx;
			posaux2+=tamx;
			posaux3+=tamx;
			posaux4+=tamx;
		}	
		posaux1+=aux;
		posaux2+=aux;
		posaux3+=aux;
		posaux4+=aux;
	}
}

void FLBitmap3::SuperSample2(unsigned short i,unsigned short j,unsigned short k,FLBitmap3& ret) const
{
	unsigned int aux;
	unsigned int contx=0;

	for (int k2=k-1;k2<k+1;k2++)
	{
		for (int j2=j-1;j2<j+1;j2++)
		{
			for (int i2=i-1;i2<i+1;i2++)
			{
				aux=0;
				ret.pixels[contx]=0.0f;
				for (int k3=k2;k3<k2+2;k3++)
				{
					for (int j3=j2;j3<j2+2;j3++)
					{
						for (int i3=i2;i3<i2+2;i3++)
						{
							if (i3>=0 && i3<tamx && j3>=0 && j3<tamy && k3>=0 && k3<tamz)
							{
								aux++;
								ret.pixels[contx]+=Pixel(i3,j3,k3);
							}
						}
					}
				}
				ret.pixels[contx]/=(float)aux;
				contx++;
			}
		}					
	}

	/*float alpha=Pixel(i,j)/((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3]+ret.pixels[4]+ret.pixels[5]+ret.pixels[6]+ret.pixels[7])*0.125f);
	for (aux=0;aux<8;aux++)
		ret.pixels[aux]*=alpha;*/

	float dif=Pixel(i,j,k)-((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3]+ret.pixels[4]+ret.pixels[5]+ret.pixels[6]+ret.pixels[7])*0.125f);
	bool yaesta[8]={false,false,false,false,false,false,false,false};
	unsigned int yaestan=0;
	unsigned int cual;

	if (dif>=0.0f)
	{
		while(yaestan<8)
		{
			float mayor=0.0f;
			cual=0;
			for (unsigned int ir=0;ir<8;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]>=mayor)
				{
					mayor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)>1.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif-1.0f)/((float)(8-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=1.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<8;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	else
	{
		while(yaestan<8)
		{
			float menor=999999.9f;
			cual=0;
			for (unsigned int ir=0;ir<8;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]<=menor)
				{
					menor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)<0.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif)/((float)(8-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=0.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<8;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	
	for (aux=0;aux<8;aux++)
	{
		if (ret.pixels[aux]<0.0f) ret.pixels[aux]=0.0f;
		if (ret.pixels[aux]>1.0f) ret.pixels[aux]=1.0f;
	}	
}

void FLBitmap3::SubImage(FLBitmap3& ret,unsigned short x,unsigned short y,unsigned short z,unsigned short auxtamx,unsigned short auxtamy,unsigned short auxtamz) const
{
	unsigned int aux2=(tamy-auxtamy)*tamx;
	unsigned int aux3=(ret.tamy-auxtamy)*ret.tamx;
	float *orig=pixels+z*tamxpertamy+y*tamx+x;
	float *dest=ret.pixels;
	for (unsigned short k=0;k<auxtamz;k++)
	{
		for (unsigned short j=0;j<auxtamy;j++)
		{
			memcpy(dest,orig,auxtamx*sizeof(float));
			orig+=tamx;
			dest+=ret.tamx;
		}
		orig+=aux2;
		dest+=aux3;
	}
}

void FLBitmap3::SubImage(FLBitmapRef3& ret,unsigned short x,unsigned short y,unsigned short z,unsigned short auxtamx,unsigned short auxtamy,unsigned short auxtamz) const
{
	ret.posx=x;
	ret.posy=y;
	ret.posz=z;
	ret.tamx=auxtamx;
	ret.tamy=auxtamy;
	ret.tamz=auxtamz;
	ret.numpixels=auxtamx*auxtamy*auxtamz;
	ret.tamxpertamy=auxtamx*auxtamy;
	ret.ref=(FLBitmap3*)this;
	/*ret.xx=xx;ret.xy=xy;ret.xz=xz;ret.yx=yx;ret.yy=yy;ret.yz=yz;ret.zx=zx;ret.zy=zy;ret.zz=zz;
	ret.ori=ori;*/
}

/*void FLBitmap3::SetOrientation(unsigned char ori2)
{
	//    0       1         2       3
	//  orig     rotx     roty     rotz
	// |1 0 0| |1  0 0| |0  0 1| |0  1 0|
	// |0 1 0| |0  0 1| |0  1 0| |-1 0 0|
	// |0 0 1| |0 -1 0| |-1 0 0| |0  0 1|

	// flips en x
	//     4       5         6        7
	// |-1 0 0| |-1 0 0| |0  0 -1| |0 -1 0|
	// |0  1 0| |0  0 1| |0  1  0| |-1 0 0|
	// |0  0 1| |0 -1 0| |-1 0  0| |0  0 1|

	// flips en y
	//     8       9        10       11
	// |1  0 0| |1 0  0| |0  0 1| |0 1 0|
	// |0 -1 0| |0 0 -1| |0 -1 0| |1 0 0|
	// |0  0 1| |0 -1 0| |-1 0 0| |0 0 1|

	// flips en z
	//     12      13     14       15
	// |1 0  0| |1 0 0| |0 0 1| |0  1  0|
	// |0 1  0| |0 0 1| |0 1 0| |-1 0  0|
	// |0 0 -1| |0 1 0| |1 0 0| |0  0 -1|

	// flips en xy
	//    16       17       18        19
	// |-1 0 0| |-1 0  0| |0  0 -1| |0 -1 0|
	// |0 -1 0| |0  0 -1| |0 -1  0| |1  0 0|
	// |0  0 1| |0 -1  0| |-1 0  0| |0  0 1|
	
	// flips en xz
	//    20       21       22        23
	// |-1 0  0| |-1 0 0| |0 0 -1| |0 -1  0|
	// |0  1  0| |0  0 1| |0 1  0| |-1 0  0|
	// |0  0 -1| |0  1 0| |1 0  0| |0  0 -1|

	// flips en yz
	//    24       25       26        27
	// |1  0  0| |1 0  0| |0  0 1| |0 1  0|
	// |0 -1  0| |0 0 -1| |0 -1 0| |1 0  0|
	// |0  0 -1| |0 1  0| |1  0 0| |0 0 -1|

	// flips en xyz
	//     28        29       30         31
	// |-1 0  0| |-1 0  0| |0  0 -1| |0 -1  0|
	// |0 -1  0| |0  0 -1| |0 -1  0| |1  0  0|
	// |0  0 -1| |0  1  0| |1  0  0| |0  0 -1|

	ori=ori2;
	switch(ori)
	{
		case 0:
			xx=yy=zz=1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 1:
			xx=yz=1;
			zy=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 2:
			xz=yy=1;
			zx=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 3:
			xy=zz=1;
			yx=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 4:
			yy=zz=1;
			xx=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 5:
			yz=1;
			xx=zy=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 6:
			yy=1;
			xz=zx=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 7:
			zz=1;
			xy=yx=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 8:
			xx=zz=1;
			yy=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 9:
			xx=1;
			yz=zy=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 10:
			xz=1;
			yy=zx=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 11:
			xy=yx=zz=1;
			xx=xz=yx=yz=zx=zy=0;
			break;
		case 12:
			xx=yy=1;
			zz=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 13:
			xx=yz=zy=1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 14:
			xz=yy=zx=1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 15:
			xy=1;
			yx=zz=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 16:
			zz=1;
			xx=yy=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 17:
			xx=yz=zy=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 18:
			xz=yy=zx=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 19:
			yx=zz=1;
			xy=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 20:
			yy=1;
			xx=zz=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 21:
			yz=zy=1;
			xx=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 22:
			yy=zx=1;
			xz=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 23:
			xy=yx=zz=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 24:
			xx=1;
			yy=zz=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 25:
			xx=zy=1;
			yz=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 26:
			xz=zx=1;
			yy=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 27:
			xy=yx=1;
			zz=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
		case 28:
			xx=yy=zz=-1;
			xy=xz=yx=yz=zx=zy=0;
			break;
		case 29:
			zy=1;
			xx=yz=-1;
			xy=xz=yx=yy=zx=zz=0;
			break;
		case 30:
			zx=1;
			xz=yy=-1;
			xx=xy=yx=yz=zy=zz=0;
			break;
		case 31:
			yx=1;
			xy=zz=-1;
			xx=xz=yy=yz=zx=zy=0;
			break;
	}
}*/

/*unsigned int FLBitmap3::SolveSums(void)
{
	float faux=0.0f;
	float sumaux1=0.0f;
	float sumaux2=0.0f;
	float sumaux3=0.0f;
	float sumaux4=0.0f;
	float sumaux5=0.0f;
	float sumaux6=0.0f;
	float sumaux7=0.0f;
	float sumaux8=0.0f;
	float sumsqraux1=0.0f;
	float sumsqraux2=0.0f;
	float sumsqraux3=0.0f;
	float sumsqraux4=0.0f;
	float sumsqraux5=0.0f;
	float sumsqraux6=0.0f;
	float sumsqraux7=0.0f;
	float sumsqraux8=0.0f;
	unsigned short tamxdiv2=tamx>>1;
	unsigned short tamydiv2=tamy>>1;
	unsigned short tamzdiv2=tamz>>1;
	register unsigned short i,j,k;

	// square1
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux1+=faux;
				sumsqraux1+=faux*faux;
			}
		}
	}
	
	// square2
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux2+=faux;
				sumsqraux2+=faux*faux;
			}
		}
	}

	// square3
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux3+=faux;
				sumsqraux3+=faux*faux;
			}
		}
	}
		
	// square4
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux4+=faux;
				sumsqraux4+=faux*faux;
			}
		}
	}

	// square5
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux5+=faux;
				sumsqraux5+=faux*faux;
			}
		}
	}
	
	// square6
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux6+=faux;
				sumsqraux6+=faux*faux;
			}
		}
	}

	// square7
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux7+=faux;
				sumsqraux7+=faux*faux;
			}
		}
	}
		
	// square8
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux8+=faux;
				sumsqraux8+=faux*faux;
			}
		}
	}
	
	// sumi, sumsqri
	sumi=sumaux1+sumaux2+sumaux3+sumaux4+sumaux5+sumaux6+sumaux7+sumaux8;
	sumsqri=sumsqraux1+sumsqraux2+sumsqraux3+sumsqraux4+sumsqraux5+sumsqraux6+sumsqraux7+sumsqraux8;

	// classify
	unsigned int classify=0;
	classify|=((sumaux1>=sumaux2)+(sumaux1>=sumaux3)+(sumaux1>=sumaux4)+(sumaux1>=sumaux5)+(sumaux1>=sumaux6)+(sumaux1>=sumaux7))<<18;
	classify|=((sumaux2>=sumaux1)+(sumaux2>=sumaux3)+(sumaux2>=sumaux4)+(sumaux2>=sumaux5)+(sumaux2>=sumaux6)+(sumaux2>=sumaux7))<<15;
	classify|=((sumaux3>=sumaux1)+(sumaux3>=sumaux2)+(sumaux3>=sumaux4)+(sumaux3>=sumaux5)+(sumaux3>=sumaux6)+(sumaux3>=sumaux7))<<12;
	classify|=((sumaux4>=sumaux1)+(sumaux4>=sumaux2)+(sumaux4>=sumaux3)+(sumaux4>=sumaux5)+(sumaux4>=sumaux6)+(sumaux4>=sumaux7))<<9;
	classify|=((sumaux5>=sumaux1)+(sumaux5>=sumaux2)+(sumaux5>=sumaux3)+(sumaux5>=sumaux4)+(sumaux5>=sumaux6)+(sumaux5>=sumaux7))<<6;
	classify|=((sumaux6>=sumaux1)+(sumaux6>=sumaux2)+(sumaux6>=sumaux3)+(sumaux6>=sumaux4)+(sumaux6>=sumaux5)+(sumaux6>=sumaux7))<<3;
	classify|=((sumaux7>=sumaux1)+(sumaux7>=sumaux2)+(sumaux7>=sumaux3)+(sumaux7>=sumaux4)+(sumaux7>=sumaux5)+(sumaux7>=sumaux6));
	return classify;
}*/

float FLBitmap3::DiferenceMean(const FLBitmap3& bit) const
{
	float dif=0.0f;
	for (unsigned int i=0;i<numpixels;i++)
		dif+=fabs(bit.pixels[i]-pixels[i]);
	return dif/(float)numpixels;
}

float FLBitmap3::DiferenceMax(const FLBitmap3& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned int i=0;i<numpixels;i++)
	{
		aux=fabs(bit.pixels[i]-pixels[i]);
		if (aux>dif) dif=aux;
	}
	return dif;
}

float FLBitmap3::DiferenceMse(const FLBitmap3& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned int i=0;i<numpixels;i++)
	{
		aux=bit.pixels[i]-pixels[i];
		dif+=aux*aux;
	}
	return dif/(float)numpixels;
}

float FLBitmap3::DiferencePsnr(const FLBitmap3& bit) const
{
	/*
	  http://pub.ufasta.edu.ar/SISD/jpeg/psnr.htm
	  El rango de los valores PSNR típicos son entre 20 y 40. 
	  Se utilizan generalmente dos decimales(Ej, 25.47). 
	  El valor real no es significativo, pero la comparación entre dos valores para diversas imágenes reconstruidas da una medida de calidad.
	  El comité del MPEG utilizó un umbral informal de 0.5 DB PSNR para decidir si incorporar una optimización de la codificación porque creyó que una mejora de esa magnitud sería visible.
	*/

	// dif es MSE - Error Cuadrático Medio
	float dif=DiferenceMse(bit);
	
	// dif es RMSE
	dif=sqrtf(dif); 
	
	// calculo el PSNR (db) peak signal-to-noise ratio
	//return 20.0f*logf(1.0f/dif)*0.43429448190325f;
	return 20.0f*log10f(1.0f/dif);
}

void FLBitmap3::Clear(void)
{
	/*float* pointer=pixels;
	for (unsigned int i=0;i<numpixels;i++)
	{
		*pointer=0.0f;
		pointer++;
	}*/
	memset(pixels,0,numpixels*sizeof(float));
}

FLBitmapRef2::FLBitmapRef2(unsigned short pgx,unsigned short pgy,unsigned short auxposx,unsigned short auxposy,unsigned short tamx2,unsigned short tamy2,FLBitmap2* ref2)
{
	posglobalx=pgx;
	posglobaly=pgy;
	posx=auxposx;
	posy=auxposy;
	xx=1;
	xy=0;
	yx=0;
	yy=1;
	ori=0;
	tamx=tamx2;
	tamy=tamy2;
	numpixels=tamx*tamy;
	ref=ref2;
	sumi=sumsqri=0.0f;
	//classify=0;
}

FLBitmapRef2::~FLBitmapRef2()
{
}

FLBitmapRef2& FLBitmapRef2::operator=(const FLBitmapRef2 &b)
{
	posx=b.posx;
	posy=b.posy;
	tamx=b.tamx;
	tamy=b.tamy;
	numpixels=tamx*tamy;
	ref=b.ref;
	xx=b.xx;
	xy=b.xy;
	yx=b.yx;
	yy=b.yy;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;
	//classify=b.classify;
	return *this;
}

bool FLBitmapRef2::operator==(const FLBitmap2 &b)
{
	float *aux2=b.pixels;
	for (unsigned int j=0;j<tamy;j++)
	{
		for (unsigned int i=0;i<tamx;i++)
		{
			if (Pixel(i,j)!=*aux2)
				return false;
			aux2++;
		}
	}
	return true;
}

bool FLBitmapRef2::operator==(const FLBitmapRef2 &b)
{
	for (unsigned int j=0;j<tamy;j++)
	{
		for (unsigned int i=0;i<tamx;i++)
		{
			if (Pixel(i,j)!=b.Pixel(i,j))
				return false;
		}
	}
	return true;
}

void FLBitmapRef2::SubSample2(FLBitmap2& ret) const
{
	unsigned int i2,j2;
	int aux1,aux2,aux3,aux4; 
	float *pointerfloat;
	for (unsigned short j=0;j<ret.tamy;j++)
	{
		j2=j<<1;
		aux3=j2*xy+posx;
		aux4=j2*yy+posy;
		pointerfloat=ret.pixels+(j*ret.tamx);
		for (unsigned short i=0;i<ret.tamx;i++)
		{
			i2=i<<1;			
			aux1=i2*xx+aux3;
			aux2=i2*yx+aux4;
			*pointerfloat=
				(
					ref->Pixel(aux1,aux2)+
					ref->Pixel(aux1+xx,aux2+yx)+
					ref->Pixel(aux1+xy,aux2+yy)+
					ref->Pixel(aux1+xx+xy,aux2+yx+yy)
				)*0.25f;
			pointerfloat++;
		}
	}	
}

void FLBitmapRef2::SuperSample2(unsigned short i,unsigned short j,FLBitmap2& ret) const
{
	unsigned int aux;
	unsigned int contx=0;

	for (int j2=j-1;j2<j+1;j2++)
	{
		for (int i2=i-1;i2<i+1;i2++)
		{
			aux=0;
			ret.pixels[contx]=0.0f;
			for (int j3=j2;j3<j2+2;j3++)
			{
				for (int i3=i2;i3<i2+2;i3++)
				{
					if (i3>=0 && i3<tamx && j3>=0 && j3<tamy)
					{
						aux++;
						ret.pixels[contx]+=Pixel(i3,j3);
					}
				}
			}
			ret.pixels[contx]/=(float)aux;
			contx++;
		}					
	}

	/*float alpha=Pixel(i,j)/((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3])*0.25f);
	for (unsigned int aux=0;aux<4;aux++)
		ret.pixels[aux]*=alpha;*/

	float dif=Pixel(i,j)-((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3])*0.25f);
	bool yaesta[4]={false,false,false,false};
	unsigned int yaestan=0;
	unsigned int cual;

	if (dif>=0.0f)
	{
		while(yaestan<4)
		{
			float mayor=0.0f;
			cual=0;
			for (unsigned int ir=0;ir<4;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]>=mayor)
				{
					mayor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)>1.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif-1.0f)/((float)(4-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=1.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<4;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	else
	{
		while(yaestan<4)
		{
			float menor=999999.9f;
			cual=0;
			for (unsigned int ir=0;ir<4;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]<=menor)
				{
					menor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)<0.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif)/((float)(4-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=0.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<4;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	
	for (aux=0;aux<4;aux++)
	{
		if (ret.pixels[aux]<0.0f) ret.pixels[aux]=0.0f;
		if (ret.pixels[aux]>1.0f) ret.pixels[aux]=1.0f;
	}	
}

void FLBitmapRef2::SubImage(FLBitmap2& ret,unsigned short x,unsigned short y,unsigned short auxtamx,unsigned short auxtamy) const
{
	float* pointeraux;
	for (unsigned short j=0;j<auxtamy;j++)
	{
		pointeraux=ret.pixels+(j*ret.tamx);
		for (unsigned short i=0;i<auxtamx;i++)
		{
			*pointeraux=Pixel(x+i,y+j);
			pointeraux++;
		}
	}
}

void FLBitmapRef2::SubImage(FLBitmapRef2& ret,unsigned short x,unsigned short y,unsigned short auxtamx,unsigned short auxtamy) const
{
	ret.posx=posx+x;
	ret.posy=posy+y;
	ret.tamx=auxtamx;
	ret.tamy=auxtamy;
	ret.numpixels=ret.tamx*ret.tamy;
	ret.ref=ref;
	ret.xx=xx;
	ret.xy=xy;
	ret.yx=yx;
	ret.yy=yy;
	ret.ori=ori;
}

void FLBitmapRef2::SetOrientation(unsigned char ori2)
{
	// xx xy
	// yx yy

	//  0      1      2      3      4      5      6      7
	// orig   90º    180º   270º   flipx   90º   180º   270º
	// |1 0| |0  1| |-1 0| |0 -1| |-1 0| |0 -1| |1 0 | |0 1| 
	// |0 1| |-1 0| |0 -1| |1  0| |0  1| |-1 0| |0 -1| |1 0|

	ori=ori2;
	switch(ori)
	{
		case 0:
			xx=yy=1;
			xy=yx=0;
			break;
		case 1:
			xy=1;
			yx=-1;
			xx=yy=0;
			break;
		case 2:
			xx=yy=-1;
			xy=yx=0;
			break;
		case 3:
			yx=1;
			xy=-1;
			xx=yy=0;
			break;
		case 4:
			yy=1;
			xx=-1;
			xy=yx=0;
			break;
		case 5:
			xy=yx=-1;
			xx=yy=0;			
			break;
		case 6:
			xx=1;
			yy=-1;
			xy=yx=0;
			break;
		case 7:
			xy=yx=1;
			xx=yy=0;			
			break;
	}

	/*int i,j;
	for (unsigned int y=0;y<tamy;y++)
	{
		for (unsigned int x=0;x<tamx;x++)
		{
			i=(x*xx+y*xy)+posx;
			j=(x*yx+y*yy)+posy;
			if (i<0 || i>=tamx || j<0 || j>=tamy)
				int jare=3;
		}
	}*/
}

unsigned int FLBitmapRef2::SolveSums(void)
{
	// optimizado: se supone que no es referencia de otra referencia *TODO*

	float faux=0.0f;
	float sumaux[4];
	sumaux[0]=sumaux[1]=sumaux[2]=sumaux[3]=0.0f;
	float sumsqraux[4];
	sumsqraux[0]=sumsqraux[1]=sumsqraux[2]=sumsqraux[3]=0.0f;
	unsigned short tamxdiv2=tamx>>1;
	unsigned short tamydiv2=tamy>>1;
	float* pixelpointer;
	short pixeljump=xx+yx*ref->tamx;
	register int i,j;

	// square1
	for (j=0;j<tamydiv2;j++)
	{
		pixelpointer=&ref->pixels[((j*yy)+posy)*ref->tamx+((j*xy)+posx)];
		for (i=0;i<tamxdiv2;i++) 
		{	
			faux=*pixelpointer;
			sumaux[0]+=faux;
			sumsqraux[0]+=faux*faux;
			pixelpointer+=pixeljump;
		}
	}
	
	// square2
	for (j=0;j<tamydiv2;j++)
	{
		pixelpointer=&ref->pixels[((tamxdiv2*yx+j*yy)+posy)*ref->tamx+((tamxdiv2*xx+j*xy)+posx)];
		for (i=tamxdiv2;i<tamx;i++) 
		{	
			faux=*pixelpointer;
			sumaux[1]+=faux;
			sumsqraux[1]+=faux*faux;
			pixelpointer+=pixeljump;
		}
	}
	
	// square3
	for (j=tamydiv2;j<tamy;j++)
	{
		pixelpointer=&ref->pixels[((j*yy)+posy)*ref->tamx+((j*xy)+posx)];
		for (i=0;i<tamxdiv2;i++) 
		{	
			faux=*pixelpointer;
			sumaux[2]+=faux;
			sumsqraux[2]+=faux*faux;
			pixelpointer+=pixeljump;
		}
	}
	
	// square4
	for (j=tamydiv2;j<tamy;j++)
	{
		pixelpointer=&ref->pixels[((tamxdiv2*yx+j*yy)+posy)*ref->tamx+((tamxdiv2*xx+j*xy)+posx)];
		for (i=tamxdiv2;i<tamx;i++) 
		{	
			faux=*pixelpointer;
			sumaux[3]+=faux;
			sumsqraux[3]+=faux*faux;
			pixelpointer+=pixeljump;
		}
	}
	
	// sumi, sumsqri
	sumi=sumaux[0]+sumaux[1]+sumaux[2]+sumaux[3];
	sumsqri=sumsqraux[0]+sumsqraux[1]+sumsqraux[2]+sumsqraux[3];
    
	// classify (0-23), calculo el índice de la permutación, relacionado con los factoradic
	unsigned int classify=0;
	unsigned char temp; 
	float tempf;
	unsigned char order[4]={0,1,2,3};
	for (i=2;i>=0;--i)
	{
		for (j=0;j<=i;++j)
		{
			if (sumaux[j]<sumaux[j+1]) 
			{
				temp=order[j+1];order[j+1]=order[j];order[j]=temp;
				tempf=sumaux[j+1];sumaux[j+1]=sumaux[j];sumaux[j]=tempf;
			}
		}
    }
    for (i=2;i>=0;--i)
	{
		for (j=0;j<=i;++j)
		{
			if (order[j]>order[j+1])
			{
				temp=order[j+1];order[j+1]=order[j];order[j]=temp;
				if (order[j]==0 || order[j+1]==0) 
					 classify+=6;
				else if (order[j]==1 || order[j+1]==1) 
					 classify+=2;
				else if (order[j]==2 || order[j+1]==2) 
					 classify+=1;
			}
		}
	}	
	return classify;
}

float FLBitmapRef2::DiferenceMean(const FLBitmapRef2& bit) const
{
	float dif=0.0f;
	for (unsigned short j=0;j<tamy;j++)
	{
		for (unsigned short i=0;i<tamx;i++)
		{
			dif+=fabs(bit.Pixel(i,j)-Pixel(i,j));
		}
	}
	return dif/(float)numpixels;
}

float FLBitmapRef2::DiferenceMax(const FLBitmapRef2& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned short j=0;j<tamy;j++)
	{
		for (unsigned short i=0;i<tamx;i++)
		{
			aux=fabs(bit.Pixel(i,j)-Pixel(i,j));
			if (aux>dif) dif=aux;
		}
	}
	return dif;
}

float FLBitmapRef2::DiferenceMse(const FLBitmapRef2& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned short j=0;j<tamy;j++)
	{
		for (unsigned short i=0;i<tamx;i++)
		{
			aux=bit.Pixel(i,j)-Pixel(i,j);
			dif+=aux*aux;
		}
	}
	return dif/(float)numpixels;
}

float FLBitmapRef2::DiferencePsnr(const FLBitmapRef2& bit) const
{
	/*
	  http://pub.ufasta.edu.ar/SISD/jpeg/psnr.htm
	  El rango de los valores PSNR típicos son entre 20 y 40. 
	  Se utilizan generalmente dos decimales(Ej, 25.47). 
	  El valor real no es significativo, pero la comparación entre dos valores para diversas imágenes reconstruidas da una medida de calidad.
	  El comité del MPEG utilizó un umbral informal de 0.5 DB PSNR para decidir si incorporar una optimización de la codificación porque creyó que una mejora de esa magnitud sería visible.
	*/

	// dif es MSE - Error Cuadrático Medio
	float dif=DiferenceMse(bit);
	
	// dif es RMSE
	dif=sqrtf(dif); 
	
	// calculo el PSNR (db) peak signal-to-noise ratio
	//return 20.0f*logf(1.0f/dif)*0.43429448190325f;
	return 20.0f*log10f(1.0f/dif);
}

void FLBitmapRef2::Clear(void)
{
	for (unsigned short j=0;j<tamy;j++)
	{
		for (unsigned short i=0;i<tamx;i++)
		{
			Pixel(i,j)=0.0f;
		}
	}
}
FLBitmapRef3::FLBitmapRef3(unsigned short pgx,unsigned short pgy,unsigned short pgz,unsigned short auxposx,unsigned short auxposy,unsigned short auxposz,unsigned short tamx2,unsigned short tamy2,unsigned short tamz2,FLBitmap3* ref2)
{
	posglobalx=pgx;
	posglobaly=pgy;
	posglobalz=pgz;
	posx=auxposx;
	posy=auxposy;
	posz=auxposz;
	xx=1;xy=0;xz=0;yx=0;yy=1;yz=0;zx=0;zy=0;zz=1;
	ori=0;
	tamx=tamx2;
	tamy=tamy2;
	tamz=tamz2;
	numpixels=tamx*tamy*tamz;
	ref=ref2;
	sumi=sumsqri=0.0f;
	tamxpertamy=tamx*tamy;
	//classify=0;
}

FLBitmapRef3::~FLBitmapRef3()
{
}

FLBitmapRef3& FLBitmapRef3::operator=(const FLBitmapRef3 &b)
{
	posx=b.posx;
	posy=b.posy;
	posz=b.posz;
	tamx=b.tamx;
	tamy=b.tamy;
	tamz=b.tamz;
	numpixels=b.numpixels;
	ref=b.ref;
	xx=b.xx;xy=b.xy;xz=b.xz;yx=b.yx;yy=b.yy;yz=b.yz;zx=b.zx;zy=b.zy;zz=b.zz;
	ori=b.ori;
	sumi=b.sumi;
	sumsqri=b.sumsqri;
	tamxpertamy=b.tamxpertamy;
	//classify=b.classify;
	return *this;
}

bool FLBitmapRef3::operator==(const FLBitmap3 &b)
{
	float *aux2=b.pixels;
	for (unsigned int k=0;k<tamz;k++)
	{
		for (unsigned int j=0;j<tamy;j++)
		{
			for (unsigned int i=0;i<tamx;i++)
			{
				if (Pixel(i,j,k)!=*aux2)
					return false;
				aux2++;
			}
		}
	}
	return true;
}

bool FLBitmapRef3::operator==(const FLBitmapRef3 &b)
{
	for (unsigned int k=0;k<tamz;k++)
	{
		for (unsigned int j=0;j<tamy;j++)
		{
			for (unsigned int i=0;i<tamx;i++)
			{
				if (Pixel(i,j,k)!=b.Pixel(i,j,k))
					return false;
			}
		}
	}
	return true;
}

void FLBitmapRef3::SubSample2(FLBitmap3& ret) const
{
	unsigned int posx,posy,posz;
	posz=0;
	for (unsigned int k=0;k<ret.tamz;k++)
	{
		posy=0;
		for (unsigned int j=0;j<ret.tamy;j++)
		{
			posx=0;
			for (unsigned int i=0;i<ret.tamx;i++)
			{
				ret.Pixel(i,j,k)=(Pixel(posx,posy,posz)+
								  Pixel(posx+1,posy,posz)+
								  Pixel(posx,posy+1,posz)+
								  Pixel(posx,posy,posz+1)+
								  Pixel(posx+1,posy+1,posz)+
								  Pixel(posx+1,posy,posz+1)+
								  Pixel(posx,posy+1,posz+1)+
								  Pixel(posx+1,posy+1,posz+1))*0.125f;
				posx+=2;
			}
			posy+=2;
		}
		posz+=2;
	}	
}

void FLBitmapRef3::SuperSample2(unsigned short i,unsigned short j,unsigned short k,FLBitmap3& ret) const
{
	unsigned int aux;
	unsigned int contx=0;

	for (int k2=k-1;k2<k+1;k2++)
	{
		for (int j2=j-1;j2<j+1;j2++)
		{
			for (int i2=i-1;i2<i+1;i2++)
			{
				aux=0;
				ret.pixels[contx]=0.0f;
				for (int k3=k2;k3<k2+2;k3++)
				{
					for (int j3=j2;j3<j2+2;j3++)
					{
						for (int i3=i2;i3<i2+2;i3++)
						{
							if (i3>=0 && i3<tamx && j3>=0 && j3<tamy && k3>=0 && k3<tamz)
							{
								aux++;
								ret.pixels[contx]+=Pixel(i3,j3,k3);
							}
						}
					}
				}
				ret.pixels[contx]/=(float)aux;
				contx++;
			}
		}					
	}

	/*float alpha=Pixel(i,j)/((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3]+ret.pixels[4]+ret.pixels[5]+ret.pixels[6]+ret.pixels[7])*0.125f);
	for (aux=0;aux<8;aux++)
		ret.pixels[aux]*=alpha;*/

	float dif=Pixel(i,j,k)-((ret.pixels[0]+ret.pixels[1]+ret.pixels[2]+ret.pixels[3]+ret.pixels[4]+ret.pixels[5]+ret.pixels[6]+ret.pixels[7])*0.125f);
	bool yaesta[8]={false,false,false,false,false,false,false,false};
	unsigned int yaestan=0;
	unsigned int cual;

	if (dif>=0.0f)
	{
		while(yaestan<8)
		{
			float mayor=0.0f;
			cual=0;
			for (unsigned int ir=0;ir<8;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]>=mayor)
				{
					mayor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)>1.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif-1.0f)/((float)(8-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=1.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<8;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	else
	{
		while(yaestan<8)
		{
			float menor=999999.9f;
			cual=0;
			for (unsigned int ir=0;ir<8;ir++)
			{
				if (!yaesta[ir] && ret.pixels[ir]<=menor)
				{
					menor=ret.pixels[ir];
					cual=ir;
				}
			}

			if ((ret.pixels[cual]+dif)<0.0f)
			{
				yaestan++;
				dif+=(ret.pixels[cual]+dif)/((float)(8-yaestan));
				yaesta[cual]=true;
				ret.pixels[cual]=0.0f;					
			}
			else
			{
				for (unsigned int ir=0;ir<8;ir++)
				{
					if (!yaesta[ir])
						ret.pixels[ir]+=dif;
				}
				break;
			}
		}
	}
	
	for (aux=0;aux<8;aux++)
	{
		if (ret.pixels[aux]<0.0f) ret.pixels[aux]=0.0f;
		if (ret.pixels[aux]>1.0f) ret.pixels[aux]=1.0f;
	}
}

void FLBitmapRef3::SubImage(FLBitmap3& ret,unsigned short x,unsigned short y,unsigned short z,unsigned short auxtamx,unsigned short auxtamy,unsigned short auxtamz) const
{
	for (unsigned int k=0;k<auxtamz;k++)
	{
		for (unsigned int j=0;j<auxtamy;j++)
		{
			for (unsigned int i=0;i<auxtamx;i++)
			{
				ret.Pixel(i,j,k)=Pixel(x+i,y+j,z+k);
			}
		}
	}
}

void FLBitmapRef3::SubImage(FLBitmapRef3& ret,unsigned short x,unsigned short y,unsigned short z,unsigned short auxtamx,unsigned short auxtamy,unsigned short auxtamz) const
{
	ret.posx=posx+x;
	ret.posy=posy+y;
	ret.posz=posz+z;
	ret.tamx=auxtamx;
	ret.tamy=auxtamy;
	ret.tamz=auxtamz;
	ret.numpixels=ret.tamx*ret.tamy*ret.tamz;
	ret.ref=ref;
	ret.xx=xx;ret.xy=xy;ret.xz=xz;ret.yx=yx;ret.yy=yy;ret.yz=yz;ret.zx=zx;ret.zy=zy;ret.zz=zz;
	ret.ori=ori;
	ret.tamxpertamy=ret.tamx*ret.tamy;
}

void FLBitmapRef3::SetOrientation(unsigned char ori2)
{
	// xx xy xz
	// yx yy yz
	// zx zy zz

	//    0       1         2       3
	//  orig     rotx     roty     rotz
	// |1 0 0| |1  0 0| |0 0 -1| |0  1 0|
	// |0 1 0| |0  0 1| |0 1  0| |-1 0 0|
	// |0 0 1| |0 -1 0| |1 0  0| |0  0 1|

	// flips en x
	//     4       5         6        7
	// |-1 0 0| |-1 0 0| |0 0 1| |0 -1 0|
	// |0  1 0| |0  0 1| |0 1 0| |-1 0 0|
	// |0  0 1| |0 -1 0| |1 0 0| |0  0 1|

	// flips en y
	//     8       9        10       11
	// |1  0 0| |1  0  0| |0  0 -1| |0 1 0|
	// |0 -1 0| |0  0 -1| |0 -1  0| |1 0 0|
	// |0  0 1| |0 -1  0| |1  0  0| |0 0 1|

	// flips en z
	//     12      13     14       15
	// |1 0  0| |1 0 0| | 0 0 -1| |0  1  0|
	// |0 1  0| |0 0 1| | 0 1  0| |-1 0  0|
	// |0 0 -1| |0 1 0| |-1 0  0| |0  0 -1|

	// flips en xy
	//    16       17       18        19
	// |-1  0 0| |-1  0  0| |0  0 1| |0 -1 0|
	// | 0 -1 0| | 0  0 -1| |0 -1 0| |1 0  0|
	// | 0  0 1| | 0 -1  0| |1  0 0| |0 0  1|
	
	// flips en xz
	//    20       21       22        23
	// |-1 0  0| |-1 0 0| | 0 0 1| |0 -1  0|
	// |0  1  0| |0  0 1| | 0 1 0| |-1 0  0|
	// |0  0 -1| |0  1 0| |-1 0 0| |0  0 -1|

	// flips en yz
	//    24       25       26        27
	// |1  0  0| |1 0  0| | 0  0 -1| |0 1  0|
	// |0 -1  0| |0 0 -1| | 0 -1  0| |1 0  0|
	// |0  0 -1| |0 1  0| |-1  0  0| |0 0 -1|

	// flips en xyz
	//     28        29       30         31
	// |-1  0  0| |-1 0  0| |0  0 1| |0 -1  0|
	// | 0 -1  0| |0  0 -1| |0 -1 0| |1  0  0|
	// | 0  0 -1| |0  1  0| |-1 0 0| |0  0 -1|

	#define matrix(m0,m1,m2,m3,m4,m5,m6,m7,m8) xx=m0;xy=m1;xz=m2;yx=m3;yy=m4;yz=m5;zx=m6;zy=m7;zz=m8;

	ori=ori2;
	switch(ori)
	{
		case 0:
			matrix(1,0,0,0,1,0,0,0,1)
			break;
		case 1:
			matrix(1,0,0,0,0,1,0,-1,0)
			break;
		case 2:
			matrix(0,0,-1,0,1,0,1,0,0)
			break;
		case 3:
			matrix(0,1,0,-1,0,0,0,0,1)
			break;
		case 4:
			matrix(-1,0,0,0,1,0,0,0,1)
			break;
		case 5:
			matrix(-1,0,0,0,0,1,0,-1,0)
			break;
		case 6:
			matrix(0,0,1,0,1,0,1,0,0)
			break;
		case 7:
			matrix(0,-1,0,-1,0,0,0,0,1)
			break;
		case 8:
			matrix(1,0,0,0,-1,0,0,0,1)
			break;
		case 9:
			matrix(1,0,0,0,0,-1,0,-1,0)
			break;
		case 10:
			matrix(0,0,-1,0,-1,0,1,0,0)
			break;
		case 11:
			matrix(0,1,0,1,0,0,0,0,1)
			break;
		case 12:
			matrix(1,0,0,0,1,0,0,0,-1)
			break;
		case 13:
			matrix(1,0,0,0,0,1,0,1,0)
			break;
		case 14:
			matrix(0,0,-1,0,1,0,-1,0,0)
			break;
		case 15:
			matrix(0,1,0,-1,0,0,0,0,-1)
			break;
		case 16:
			matrix(-1,0,0,0,-1,0,0,0,1)
			break;
		case 17:
			matrix(-1,0,0,0,0,-1,0,-1,0)
			break;
		case 18:
			matrix(0,0,1,0,-1,0,1,0,0)
			break;
		case 19:
			matrix(0,-1,0,1,0,0,0,0,1)
			break;
		case 20:
			matrix(-1,0,0,0,1,0,0,0,-1)
			break;
		case 21:
			matrix(-1,0,0,0,0,1,0,1,0)
			break;
		case 22:
			matrix(0,0,1,0,1,0,-1,0,0)
			break;
		case 23:
			matrix(0,-1,0,-1,0,0,0,0,-1)
			break;
		case 24:
			matrix(1,0,0,0,-1,0,0,0,-1)
			break;
		case 25:
			matrix(1,0,0,0,0,-1,0,1,0)
			break;
		case 26:
			matrix(0,0,-1,0,-1,0,-1,0,0)
			break;
		case 27:
			matrix(0,1,0,1,0,0,0,0,-1)
			break;
		case 28:
			matrix(-1,0,0,0,-1,0,0,0,-1)
			break;
		case 29:
			matrix(-1,0,0,0,0,-1,0,1,0)
			break;
		case 30:
			matrix(0,0,1,0,-1,0,-1,0,0)
			break;
		case 31:
			matrix(0,-1,0,1,0,0,0,0,-1)
			break;
	}

	/*int i,j,k;
	for (unsigned int z=0;z<tamz;z++)
	{
		for (unsigned int y=0;y<tamy;y++)
		{
			for (unsigned int x=0;x<tamx;x++)
			{
				i=((x*xx+y*xy+z*xz)+posx)-posglobalx/2;
				j=((x*yx+y*yy+z*yz)+posy)-posglobaly/2;
				k=((x*zx+y*zy+z*zz)+posz)-posglobalz/2;
				if (i<0 || i>=tamx || j<0 || j>=tamy || k<0 || k>=tamz)
					int jare=3;
			}
		}
	}*/
}

unsigned int FLBitmapRef3::SolveSums(void)
{
	float faux=0.0f;
	float sumaux[8];
	sumaux[0]=sumaux[1]=sumaux[2]=sumaux[3]=sumaux[4]=sumaux[5]=sumaux[6]=sumaux[7]=0.0f;
	float sumsqraux[8];
	sumsqraux[0]=sumsqraux[1]=sumsqraux[2]=sumsqraux[3]=sumsqraux[4]=sumsqraux[5]=sumsqraux[6]=sumsqraux[7]=0.0f;
	unsigned short tamxdiv2=tamx>>1;
	unsigned short tamydiv2=tamy>>1;
	unsigned short tamzdiv2=tamz>>1;
	register int i,j,k;

	// square1
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[0]+=faux;
				sumsqraux[0]+=faux*faux;
			}
		}
	}
	
	// square2
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[1]+=faux;
				sumsqraux[1]+=faux*faux;
			}
		}
	}

	// square3
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[2]+=faux;
				sumsqraux[2]+=faux*faux;
			}
		}
	}
		
	// square4
	for (k=0;k<tamzdiv2;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[3]+=faux;
				sumsqraux[3]+=faux*faux;
			}
		}
	}

	// square5
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[4]+=faux;
				sumsqraux[4]+=faux*faux;
			}
		}
	}
	
	// square6
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=0;j<tamydiv2;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[5]+=faux;
				sumsqraux[5]+=faux*faux;
			}
		}
	}

	// square7
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=0;i<tamxdiv2;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[6]+=faux;
				sumsqraux[6]+=faux*faux;
			}
		}
	}
		
	// square8
	for (k=tamzdiv2;k<tamz;k++)
	{
		for (j=tamydiv2;j<tamy;j++)
		{
			for (i=tamxdiv2;i<tamx;i++) 
			{	
				faux=Pixel(i,j,k);
				sumaux[7]+=faux;
				sumsqraux[7]+=faux*faux;
			}
		}
	}
	
	// sumi, sumsqri
	sumi=sumaux[0]+sumaux[1]+sumaux[2]+sumaux[3]+sumaux[4]+sumaux[5]+sumaux[6]+sumaux[7];
	sumsqri=sumsqraux[0]+sumsqraux[1]+sumsqraux[2]+sumsqraux[3]+sumsqraux[4]+sumsqraux[5]+sumsqraux[6]+sumsqraux[7];

	// classify (0-40320), calculo el índice de la permutación, relacionado con los factoradic
	unsigned int classify=0;
	unsigned char temp; 
	float tempf;
	unsigned char order[8]={0,1,2,3,4,5,6,7};
	for (i=6;i>=0;--i)
	{
		for (j=0;j<=i;++j)
		{
			if (sumaux[j]<sumaux[j+1]) 
			{
				temp=order[j+1];order[j+1]=order[j];order[j]=temp;
				tempf=sumaux[j+1];sumaux[j+1]=sumaux[j];sumaux[j]=tempf;
			}
		}
    }
    for (i=6;i>=0;--i)
	{
		for (j=0;j<=i;++j)
		{
			if (order[j]>order[j+1])
			{
				temp=order[j+1];order[j+1]=order[j];order[j]=temp;
				if (order[j]==0 || order[j+1]==0) 
					 classify+=5040;
				else if (order[j]==1 || order[j+1]==1) 
					 classify+=720;
				else if (order[j]==2 || order[j+1]==2) 
					 classify+=120;
				else if (order[j]==3 || order[j+1]==3) 
					 classify+=24;
				else if (order[j]==4 || order[j+1]==4) 
					 classify+=6;
				else if (order[j]==5 || order[j+1]==5) 
					 classify+=2;
				else if (order[j]==6 || order[j+1]==6) 
					 classify+=1;
			}
		}
	}	
	return classify;
}


float FLBitmapRef3::DiferenceMean(const FLBitmapRef3& bit) const
{
	float dif=0.0f;
	for (unsigned short k=0;k<tamz;k++)
	{
		for (unsigned short j=0;j<tamy;j++)
		{
			for (unsigned short i=0;i<tamx;i++)
			{
				dif+=fabs(bit.Pixel(i,j,k)-Pixel(i,j,k));
			}
		}
	}
	return dif/(float)numpixels;
}

float FLBitmapRef3::DiferenceMax(const FLBitmapRef3& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned short k=0;k<tamz;k++)
	{
		for (unsigned short j=0;j<tamy;j++)
		{
			for (unsigned short i=0;i<tamx;i++)
			{
				aux=fabs(bit.Pixel(i,j,k)-Pixel(i,j,k));
				if (aux>dif) dif=aux;
			}
		}
	}
	return dif;
}

float FLBitmapRef3::DiferenceMse(const FLBitmapRef3& bit) const
{
	float dif=0.0f;
	float aux;
	for (unsigned short k=0;k<tamz;k++)
	{
		for (unsigned short j=0;j<tamy;j++)
		{
			for (unsigned short i=0;i<tamx;i++)
			{
				aux=bit.Pixel(i,j,k)-Pixel(i,j,k);
				dif+=aux*aux;
			}
		}
	}
	return dif/(float)numpixels;
}

float FLBitmapRef3::DiferencePsnr(const FLBitmapRef3& bit) const
{
	/*
	  http://pub.ufasta.edu.ar/SISD/jpeg/psnr.htm
	  El rango de los valores PSNR típicos son entre 20 y 40. 
	  Se utilizan generalmente dos decimales(Ej, 25.47). 
	  El valor real no es significativo, pero la comparación entre dos valores para diversas imágenes reconstruidas da una medida de calidad.
	  El comité del MPEG utilizó un umbral informal de 0.5 DB PSNR para decidir si incorporar una optimización de la codificación porque creyó que una mejora de esa magnitud sería visible.
	*/

	// dif es MSE - Error Cuadrático Medio
	float dif=DiferenceMse(bit);
	
	// dif es RMSE
	dif=sqrtf(dif); 
	
	// calculo el PSNR (db) peak signal-to-noise ratio
	//return 20.0f*logf(1.0f/dif)*0.43429448190325f;
	return 20.0f*log10f(1.0f/dif);
}

void FLBitmapRef3::Clear(void)
{
	for (unsigned short k=0;k<tamz;k++)
	{
		for (unsigned short j=0;j<tamy;j++)
		{
			for (unsigned short i=0;i<tamx;i++)
			{
				Pixel(i,j,k)=0.0f;
			}
		}
	}
}