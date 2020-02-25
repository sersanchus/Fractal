#ifndef __FLDOMAINS_H__
#define __FLDOMAINS_H__

#include "FLTrans.h"
#include "FLBitmap.h"
#include <vector>
#include <map>

using namespace std;

class FRACTALLIB_API FLDomains2
{

private:

	FLBitmap2 *bitmap2x2[4];
	unsigned short domsize;
	unsigned short domsalto;

	bool AddIfDifferent(FLBitmapRef2*);

public:

	vector<FLBitmapRef2*> domain[24];
	map<float,vector<FLBitmapRef2*> > domainorder;

	FLDomains2(FLBitmap2*,unsigned short,unsigned short);
	~FLDomains2();

	void OptimizeTrans(FLTrans2*,FLBitmapRef2&);

};

class FRACTALLIB_API FLDomains3
{

private:

	FLBitmap3 *bitmap2x2[8];
	unsigned short domsize;
	unsigned short domsalto;

	bool AddIfDifferent(FLBitmapRef3*);

public:

	vector<FLBitmapRef3*> domain[40320];
	map<float,vector<FLBitmapRef3*> > domainorder;

	FLDomains3(FLBitmap3*,unsigned short,unsigned short);
	~FLDomains3();

	void OptimizeTrans(FLTrans3*,FLBitmapRef3&);

};

#endif