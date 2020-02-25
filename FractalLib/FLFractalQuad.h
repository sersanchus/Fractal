#ifndef __FLFRACTALQUAD_H__
#define __FLFRACTALQUAD_H__

#include "FLDefs.h"
#include "FLFractal.h"
#include "FLBitmap.h"
#include "FLTransQuad.h"

class FRACTALLIB_API FLFractalQuad2 : public FLFractal2
{

public:

	//unsigned short nummaxtransformations;

	unsigned int numbasetrans;

	FLFractalQuad2(void);
	~FLFractalQuad2();

	// semilla bitmap y número de pasos
	void DecodeRecurs(FLTransQuad2*,FLBitmap2*,unsigned int) const;
	FLBitmap2 Decode(const FLBitmap2&,char z=0,unsigned int npasos=1) const;
	
	//float Decode(const FLVector2&,unsigned int) const;
	float Decode(const FLVector2&,float,float,unsigned int&) const;
	void DrawBoundsRecurs(FLTransQuad2*,FLBitmap2*,char) const;
	void DrawBounds(FLBitmap2&,char z=0) const;
	void SmoothBoundsRecurs(FLTransQuad2*,FLBitmap2*,FLBitmap2*,char) const;
	void SmoothBounds(FLBitmap2&,char z=0) const;

	void Encode(FLBitmap2*,unsigned short,unsigned int);
	void Save(FLFile&) const;
	void Load(FLFile&);

};

class FRACTALLIB_API FLFractalQuad3 : public FLFractal3
{

public:

	//unsigned short nummaxtransformations;

	unsigned int numbasetrans;

	FLFractalQuad3(void);
	~FLFractalQuad3();

	// semilla bitmap y número de pasos
	void DecodeRecurs(FLTransQuad3*,FLBitmap3*,unsigned int) const;
	FLBitmap3 Decode(const FLBitmap3&,char z=0,unsigned int npasos=1) const;
	
	//float Decode(const FLVector2&,unsigned int) const;
	float Decode(const FLVector3&,float,float,unsigned int&) const;
	void DrawBoundsRecurs(FLTransQuad3*,FLBitmap3*,char) const;
	void DrawBounds(FLBitmap3&,char z=0) const;
	void SmoothBoundsRecurs(FLTransQuad3*,FLBitmap3*,FLBitmap3*,char) const;
	void SmoothBounds(FLBitmap3&,char z=0) const;

	void Encode(FLBitmap3*,unsigned short,unsigned int);
	void Save(FLFile&) const;
	void Load(FLFile&);

};

#endif