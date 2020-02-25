#include "FLFractal.h"
#include "FLFractalConst.h"
#include "FLFractalQuad.h"
/*#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>*/

//using namespace std;

FLFractal2::FLFractal2(/*unsigned char ty*/)
{
	//type=ty;
	imagetamx=256;
	imagetamy=256;
	transformations=NULL;
	numtransformations=0;
}

FLFractal2::~FLFractal2()
{	
}

FLFractal3::FLFractal3(/*unsigned char ty*/)
{
	//type=ty;
	imagetamx=256;
	imagetamy=256;
	imagetamz=256;
	transformations=NULL;
	numtransformations=0;
}

FLFractal3::~FLFractal3()
{	
}

unsigned short aplicazoom(unsigned short x,char zoom)
{
	if (zoom>=0)
		return x<<zoom;
	else
		return x>>(-zoom);
}