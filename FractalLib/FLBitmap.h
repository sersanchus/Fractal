#ifndef __FLBITMAP_H__
#define __FLBITMAP_H__

#include "FLDefs.h"

class FLBitmapRef2;

class FRACTALLIB_API FLBitmap2
{

public:
	
	unsigned short posglobalx;
	unsigned short posglobaly;

	float *pixels;
	unsigned int numpixels;
	unsigned short tamx;
	unsigned short tamy;

	/*char xx,xy;
	char yx,yy;
	unsigned char ori;*/

	FLBitmap2(void);
	FLBitmap2(const FLBitmap2&);
	FLBitmap2(unsigned short);
	FLBitmap2(unsigned short,unsigned short);
	~FLBitmap2();
	
	FLBitmap2& operator=(const FLBitmap2&);
	FLBitmap2& operator=(const FLBitmapRef2&);
	bool operator==(const FLBitmap2&);
	bool operator==(const FLBitmapRef2&);
	
	inline float &Pixel(unsigned short x,unsigned short y) const {return pixels[y*tamx+x];}

	void SubSample2(FLBitmap2&) const;
	void SuperSample2(unsigned short,unsigned short,FLBitmap2&) const;
	void SubImage(FLBitmap2&,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SubImage(FLBitmapRef2&,unsigned short,unsigned short,unsigned short,unsigned short) const;
	//void SetOrientation(unsigned char);

	//float sumi,sumsqri;
	//unsigned char classify;
	//unsigned int SolveSums(void);

	float DiferenceMean(const FLBitmap2&) const;
	float DiferenceMax(const FLBitmap2&) const;
	float DiferenceMse(const FLBitmap2&) const;
	float DiferencePsnr(const FLBitmap2&) const;

	void Clear(void);

};

class FLBitmapRef3;

class FRACTALLIB_API FLBitmap3
{

public:
	
	unsigned short posglobalx;
	unsigned short posglobaly;
	unsigned short posglobalz;

	float *pixels;
	unsigned int numpixels;
	unsigned int tamxpertamy;
	unsigned short tamx;
	unsigned short tamy;
	unsigned short tamz;

	/*char xx,xy,xz;
	char yx,yy,yz;
	char zx,zy,zz;
	unsigned char ori;*/

	FLBitmap3(void);
	FLBitmap3(const FLBitmap3&);
	FLBitmap3(unsigned short);
	FLBitmap3(unsigned short,unsigned short,unsigned short);
	~FLBitmap3();
	
	FLBitmap3& operator=(const FLBitmap3&);
	FLBitmap3& operator=(const FLBitmapRef3&);
	bool operator==(const FLBitmap3&);
	bool operator==(const FLBitmapRef3&);
	
	inline float &Pixel(unsigned short x,unsigned short y,unsigned short z) const {return pixels[z*tamxpertamy+y*tamx+x];}

	void SubSample2(FLBitmap3&) const;
	void SuperSample2(unsigned short,unsigned short,unsigned short,FLBitmap3&) const;
	void SubImage(FLBitmap3&,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SubImage(FLBitmapRef3&,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short) const;
	//void SetOrientation(unsigned char);

	//float sumi,sumsqri;
	//unsigned char classify;
	//unsigned int SolveSums(void);

	float DiferenceMean(const FLBitmap3&) const;
	float DiferenceMax(const FLBitmap3&) const;
	float DiferenceMse(const FLBitmap3&) const;
	float DiferencePsnr(const FLBitmap3&) const;

	void Clear(void);

};

class FRACTALLIB_API FLBitmapRef2
{

public:

	unsigned short posglobalx;
	unsigned short posglobaly;

	unsigned int numpixels;
	unsigned short tamx;
	unsigned short tamy;

	char xx,xy;
	char yx,yy;
	unsigned char ori;

	FLBitmap2* ref;
	unsigned short posx;
	unsigned short posy;

	float sumi,sumsqri;
	//unsigned char classify;
	
	FLBitmapRef2(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,FLBitmap2*);
	~FLBitmapRef2();
	
	FLBitmapRef2& operator=(const FLBitmapRef2&);
	bool operator==(const FLBitmap2&);
	bool operator==(const FLBitmapRef2&);
	
	inline float &Pixel(unsigned short x,unsigned short y) const {return ref->Pixel((x*xx+y*xy)+posx,(x*yx+y*yy)+posy);}

	void SubSample2(FLBitmap2&) const;
	void SuperSample2(unsigned short,unsigned short,FLBitmap2&) const;
	void SubImage(FLBitmap2&,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SubImage(FLBitmapRef2&,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SetOrientation(unsigned char);

	unsigned int SolveSums(void);

	float DiferenceMean(const FLBitmapRef2&) const;
	float DiferenceMax(const FLBitmapRef2&) const;
	float DiferenceMse(const FLBitmapRef2&) const;
	float DiferencePsnr(const FLBitmapRef2&) const;

	void Clear(void);

};

class FRACTALLIB_API FLBitmapRef3
{

public:

	unsigned short posglobalx;
	unsigned short posglobaly;
	unsigned short posglobalz;

	unsigned int numpixels;
	unsigned int tamxpertamy;
	unsigned short tamx;
	unsigned short tamy;
	unsigned short tamz;

	char xx,xy,xz;
	char yx,yy,yz;
	char zx,zy,zz;
	unsigned char ori;

	FLBitmap3* ref;
	unsigned short posx;
	unsigned short posy;
	unsigned short posz;

	float sumi,sumsqri;
	//unsigned char classify;
	
	FLBitmapRef3(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,FLBitmap3*);
	~FLBitmapRef3();
	
	FLBitmapRef3& operator=(const FLBitmapRef3&);
	bool operator==(const FLBitmap3&);
	bool operator==(const FLBitmapRef3&);
	
	inline float &Pixel(unsigned short x,unsigned short y,unsigned short z) const {return ref->Pixel((x*xx+y*xy+z*xz)+posx,(x*yx+y*yy+z*yz)+posy,(x*zx+y*zy+z*zz)+posz);}

	void SubSample2(FLBitmap3&) const;
	void SuperSample2(unsigned short,unsigned short,unsigned short,FLBitmap3&) const;
	void SubImage(FLBitmap3&,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SubImage(FLBitmapRef3&,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short) const;
	void SetOrientation(unsigned char);

	unsigned int SolveSums(void);

	float DiferenceMean(const FLBitmapRef3&) const;
	float DiferenceMax(const FLBitmapRef3&) const;
	float DiferenceMse(const FLBitmapRef3&) const;
	float DiferencePsnr(const FLBitmapRef3&) const;

	void Clear(void);

};

#endif