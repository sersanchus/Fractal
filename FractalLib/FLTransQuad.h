#ifndef __FLTRANSQUAD_H__
#define __FLTRANSQUAD_H__

#include "FLDefs.h"
#include "FLTrans.h"

class FRACTALLIB_API FLTransQuad2 : public FLTrans2
{

public:

	bool splitted;
	FLTransQuad2 *child[4];

	FLTransQuad2();
	FLTransQuad2(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTransQuad2();

	void Save(FLFile&);
	void Load(FLFile&,unsigned short,unsigned short,unsigned char);

};

class FRACTALLIB_API FLTransQuad3 : public FLTrans3
{

public:

	bool splitted;
	FLTransQuad3 *child[8];

	FLTransQuad3();
	FLTransQuad3(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
	~FLTransQuad3();

	void Save(FLFile&);
	void Load(FLFile&,unsigned short,unsigned short,unsigned short,unsigned char);

};

#endif