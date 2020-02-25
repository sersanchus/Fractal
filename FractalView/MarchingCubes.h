#ifndef __MARCHINGCUBES_H__
#define __MARCHINGCUBES_H__

#if defined(_MSC_VER)
	#include "../FractalLib/FLVector.h"
#else
	#include "FLVector.h"
#endif

class MarchingCubes
{

private:

	unsigned short tam;
	float voxelsizeinv;
	unsigned char *voxels;

	unsigned int marchigcubesdisplaylist;
		
	FLVector3 RealPos(float,float,float);
	void MarchCube(unsigned int,unsigned int,unsigned int);

public:	
	
	MarchingCubes(unsigned short);
	MarchingCubes(unsigned short,char*);
	~MarchingCubes(void);

	unsigned int numtriangles;

	void Draw(void);
	void Load(unsigned char*);

};

#endif