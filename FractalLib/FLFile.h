#ifndef __FLFILE_H__
#define __FLFILE_H__

#include "FLDefs.h"

#ifdef _MSC_VER
	#include "zlib/zlib.h"
#else
	#include <zlib.h>
#endif

class FRACTALLIB_API FLFile
{

private:

	unsigned char ptrunpack;
	unsigned short sumunpack;
	unsigned short lenunpack; // en bytes
	unsigned char ptrpack;
	unsigned short sumpack;
	unsigned short lenpack; // en bytes

public:

	gzFile file;

	FLFile(char*,char*);
	~FLFile();

	int Pack(int,long);
	long Unpack(int);

	void Close(void);
	void Open(char*,char*);

};

#endif
