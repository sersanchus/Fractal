#ifndef __FLFRACTALCONST_H__
#define __FLFRACTALCONST_H__

#include "FLDefs.h"
#include "FLFractal.h"
#include "FLBitmap.h"
#include "FLTransConst.h"

class FRACTALLIB_API FLFractalConst2 : public FLFractal2
{

public:

	unsigned short ransize;

	FLFractalConst2(void);
	~FLFractalConst2();

	// semilla bitmap, zoom y número de pasos
	FLBitmap2 Decode(const FLBitmap2&,char z=0,unsigned int npasos=1) const;
	
	//float Decode(const FLVector2&,unsigned int) const;
	float Decode(const FLVector2&,float,float,unsigned int&) const;
	void DrawBounds(FLBitmap2&,char z=0) const;
	void SmoothBounds(FLBitmap2&,char z=0) const;

	void Encode(FLBitmap2*,unsigned short,unsigned short);
	void Save(FLFile&) const;
	void Load(FLFile&);

};

class FRACTALLIB_API FLFractalConst3 : public FLFractal3
{

public:

	unsigned short ransize;

	FLFractalConst3(void);
	~FLFractalConst3();

	// semilla bitmap, zoom y número de pasos
	FLBitmap3 Decode(const FLBitmap3&,char z=0,unsigned int npasos=1) const;
	
	//float Decode(const FLVector2&,unsigned int) const;
	float Decode(const FLVector3&,float,float,unsigned int&) const;
	void DrawBounds(FLBitmap3&,char z=0) const;
	void SmoothBounds(FLBitmap3&,char z=0) const;

	void Encode(FLBitmap3*,unsigned short,unsigned short);
	void Save(FLFile&) const;
	void Load(FLFile&);

};

#endif