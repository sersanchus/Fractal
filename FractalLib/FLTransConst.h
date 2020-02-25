#ifndef __FLTRANSCONST_H__
#define __FLTRANSCONST_H__

#include "FLDefs.h"
#include "FLTrans.h"

class FRACTALLIB_API FLTransConst2 : public FLTrans2
{

public:

	FLTransConst2();
	FLTransConst2(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTransConst2();

	void Save(FLFile&);
	void Load(FLFile&);

};

class FRACTALLIB_API FLTransConst3 : public FLTrans3
{

public:

	FLTransConst3();
	FLTransConst3(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTransConst3();

	void Save(FLFile&);
	void Load(FLFile&);

};

#endif