#ifndef __FLTRANS_H__
#define __FLTRANS_H__

#include "FLDefs.h"
#include "FLBitmap.h"
#include "FLFile.h"

class FRACTALLIB_API FLTrans2
{

public:

	// dominio
	unsigned short domposx,domposy,domsize;
	// rango
	unsigned short ranposx,ranposy,ransize;
	// orientación
	unsigned char ori;
	// contraste	
	float s;
	unsigned char pis;
	// brillo
	float o;
	unsigned char pio;
	// diferencia
	float rms;
	//float dif;

	FLTrans2();
	FLTrans2(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTrans2();

	FLTrans2& operator=(const FLTrans2&);

	//float LeastSquares(const FLBitmapRef&,const FLBitmapRef&,float&,float&,int&,int&);
	//void OptimizeTrans(const FLBitmapRef&,FLBitmapRef**,unsigned int);	

};

class FRACTALLIB_API FLTrans3
{

public:

	// dominio
	unsigned short domposx,domposy,domposz,domsize;
	// rango
	unsigned short ranposx,ranposy,ranposz,ransize;
	// orientación
	unsigned char ori;
	// contraste	
	float s;
	unsigned char pis;
	// brillo
	float o;
	unsigned char pio;
	// diferencia
	float rms;
	//float dif;

	FLTrans3();
	FLTrans3(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTrans3();

	FLTrans3& operator=(const FLTrans3&);

	//float LeastSquares(const FLBitmapRef&,const FLBitmapRef&,float&,float&,int&,int&);
	//void OptimizeTrans(const FLBitmapRef&,FLBitmapRef**,unsigned int);	

};

#endif