#ifndef __FLFRACTAL_H__
#define __FLFRACTAL_H__

#include "FLDefs.h"
#include "FLTrans.h"
#include "FLBitmap.h"
#include "FLVector.h"

#define FRACTAL_2D	0
#define FRACTAL_3D	1

#define FRACTAL_CONST	0
#define FRACTAL_QUAD	1

#define FRACTAL_GRAYSCALE	0
#define FRACTAL_TRUECOLOR	1

class FRACTALLIB_API FLFractal2
{

public:

	//unsigned char type;

	FLTrans2** transformations;
	unsigned int numtransformations;

	unsigned short imagetamx;
	unsigned short imagetamy;

	FLFractal2(/*unsigned char*/);
	~FLFractal2(void);
	
	// semilla bitmap y número de pasos
	virtual FLBitmap2 Decode(const FLBitmap2&,char z=0,unsigned int npasos=1) const =0; 
	
	// decodifica n pasos de una posición en [0-1][0-1]
	// a partir de blanco
	//virtual float Decode(const FLVector2&,unsigned int) const =0;
	virtual float Decode(const FLVector2&,float,float,unsigned int&) const =0;

	virtual void DrawBounds(FLBitmap2&,char z=0) const =0;
	virtual void SmoothBounds(FLBitmap2&,char z=0) const =0;
	
	//virtual void Encode(FLBitmap2*)=0;
	virtual void Save(FLFile&) const=0;
	virtual void Load(FLFile&)=0;

};

class FRACTALLIB_API FLFractal3
{

public:

	//unsigned char type;

	FLTrans3** transformations;
	unsigned int numtransformations;

	unsigned short imagetamx;
	unsigned short imagetamy;
	unsigned short imagetamz;

	FLFractal3(/*unsigned char*/);
	~FLFractal3(void);
	
	// semilla bitmap y número de pasos
	virtual FLBitmap3 Decode(const FLBitmap3&,char z=0,unsigned int npasos=1) const =0; 
	
	// decodifica n pasos de una posición en [0-1][0-1]
	// a partir de blanco
	//virtual float Decode(const FLVector2&,unsigned int) const =0;
	virtual float Decode(const FLVector3&,float,float,unsigned int&) const =0;

	virtual void DrawBounds(FLBitmap3&,char z=0) const =0;
	virtual void SmoothBounds(FLBitmap3&,char z=0) const =0;
	
	//virtual void Encode(FLBitmap2*)=0;
	virtual void Save(FLFile&) const=0;
	virtual void Load(FLFile&)=0;

};

unsigned short aplicazoom(unsigned short x,char zoom);

#endif