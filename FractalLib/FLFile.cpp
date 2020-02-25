#include "FLFile.h"

FLFile::FLFile(char *ruta,char *modo)
{
	file=gzopen(ruta,modo);
	gzsetparams(file,Z_BEST_COMPRESSION,Z_HUFFMAN_ONLY);
	sumunpack=0;
	ptrunpack=0;
	lenunpack=0;
	sumpack=0;
	ptrpack=0;
	lenpack=0;
}

FLFile::~FLFile()
{
	if (file)
	{
		if (ptrpack!=0)
		{
			gzputc(file,sumpack<<(7-ptrpack));
			lenpack++;
		}
		gzclose(file);
	}
}

int FLFile::Pack(int size,long int value)
{
	int i;
    
    for (i=size-1; i>=0; i--, sumpack<<=1) 
	{
		if (value&(1<<i)) 
			sumpack|=1; 

		if (ptrpack==7) 
		{
            gzputc(file,sumpack);
            lenpack++;
            sumpack=0;
            ptrpack=0;
        }
		else
		{
			ptrpack++;
		}
    }
	return ((long)value);
}

long FLFile::Unpack(int size)
{
    int i;
    int value=0;

    for (i=0; i<size; i++, sumunpack<<=1) 
	{
		value<<=1;

        if (ptrunpack==0) 
		{
            sumunpack=gzgetc(file);
			lenunpack++;
            ptrunpack=7;
        }
		else
		{
			ptrunpack--;
		}

		if (sumunpack&128) 
			value|=1;
    }
    return((long)value);
}

void FLFile::Close(void)
{
	if (file)
	{
		if (ptrpack!=0)
		{
			gzputc(file,sumpack<<(7-ptrpack));
			lenpack++;
		}
		gzclose(file);
		file=NULL;
	}
}

void FLFile::Open(char *ruta,char *modo)
{
	file=gzopen(ruta,modo);
	gzsetparams(file,Z_BEST_COMPRESSION,Z_HUFFMAN_ONLY);
	sumunpack=0;
	ptrunpack=0;
	lenunpack=0;
	sumpack=0;
	ptrpack=0;
	lenpack=0;
}