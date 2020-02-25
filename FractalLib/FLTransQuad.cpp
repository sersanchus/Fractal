#include "FLTransQuad.h"
#include <math.h>

#ifndef NULL
	#define NULL 0x00000000
#endif

FLTransQuad2::FLTransQuad2()
{
	splitted=false;
	child[0]=child[1]=child[2]=child[3]=NULL;
}

FLTransQuad2::FLTransQuad2(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxransize)
:FLTrans2(auxdomposx,auxdomposy,auxdomsize,auxranposx,auxranposy,auxransize)
{
	splitted=false;
	child[0]=child[1]=child[2]=child[3]=NULL;
}

FLTransQuad2::~FLTransQuad2()
{
	if (splitted)
	{
		delete child[0];
		delete child[1];
		delete child[2];
		delete child[3];
	}
}

void FLTransQuad2::Save(FLFile &file)
{
	// peor caso: 6 bytes = 48 bits 
	// mejor caso: 1 bit

	// splitted 1 bit
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits
    
	file.Pack(1,splitted);
	if (splitted)
	{
		child[0]->Save(file);
		child[1]->Save(file);
		child[2]->Save(file);
		child[3]->Save(file);
	}
	else
	{
		file.Pack(5,pis);
		file.Pack(7,pio);
		if (pis!=16)
		{
			file.Pack(16,domposx);
			file.Pack(16,domposy);			
			file.Pack(3,ori);
		}	
	}
}

void FLTransQuad2::Load(FLFile &file,unsigned short ranx,unsigned short rany,unsigned char rans)
{
	// peor caso: 6 bytes = 48 bits 
	// mejor caso: 1 bit

	// splitted 1 bit
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits

	splitted=file.Unpack(1)==1;
	if (splitted)
	{
		child[0]=new FLTransQuad2();
		child[0]->Load(file,ranx,rany,rans>>1);
		child[1]=new FLTransQuad2();
		child[1]->Load(file,ranx+(rans>>1),rany,rans>>1);
		child[2]=new FLTransQuad2();
		child[2]->Load(file,ranx,rany+(rans>>1),rans>>1);
		child[3]=new FLTransQuad2();
		child[3]->Load(file,ranx+(rans>>1),rany+(rans>>1),rans>>1);
	}
	else
	{
		pis=(unsigned char)file.Unpack(5);
		pio=(unsigned char)file.Unpack(7);
		//s=(float)pis/(float)(1<<5)*2.0f-1.0f;
		s=(float)pis*0.0625f-1.0f;
	    //o=(float)pio/(float)((1<<7)-1)*((1.0f+fabs(s))*255.0f);
		o=(float)pio*0.0078740157480314f*(1.0f+fabs(s));
		if (s>0.0) o-=s;
		ranposx=ranx;
		ranposy=rany;
		ransize=rans;
		domsize=rans<<1;
		if (pis!=16)
		{
			domposx=(unsigned short)file.Unpack(16);
			domposy=(unsigned short)file.Unpack(16);
			ori=(unsigned char)file.Unpack(3);
		}	
	}
}

FLTransQuad3::FLTransQuad3()
{
	splitted=false;
	child[0]=child[1]=child[2]=child[3]=child[4]=child[5]=child[6]=child[7]=NULL;
}

FLTransQuad3::FLTransQuad3(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomposz, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxranposz, unsigned short auxransize)
:FLTrans3(auxdomposx,auxdomposy,auxdomposz,auxdomsize,auxranposx,auxranposy,auxranposz,auxransize)
{
	splitted=false;
	child[0]=child[1]=child[2]=child[3]=child[4]=child[5]=child[6]=child[7]=NULL;
}

FLTransQuad3::~FLTransQuad3()
{
	if (splitted)
	{
		delete child[0];
		delete child[1];
		delete child[2];
		delete child[3];
		delete child[4];
		delete child[5];
		delete child[6];
		delete child[7];
	}
}

void FLTransQuad3::Save(FLFile &file)
{
	// peor caso: 6 bytes = 48 bits 
	// mejor caso: 1 bit

	// splitted 1 bit
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits
    
	file.Pack(1,splitted);
	if (splitted)
	{
		child[0]->Save(file);
		child[1]->Save(file);
		child[2]->Save(file);
		child[3]->Save(file);
		child[4]->Save(file);
		child[5]->Save(file);
		child[6]->Save(file);
		child[7]->Save(file);
	}
	else
	{
		file.Pack(5,pis);
		file.Pack(7,pio);
		if (pis!=16)
		{
			file.Pack(16,domposx);
			file.Pack(16,domposy);			
			file.Pack(16,domposz);			
			file.Pack(5,ori);
		}	
	}
}

void FLTransQuad3::Load(FLFile &file,unsigned short ranx,unsigned short rany,unsigned short ranz,unsigned char rans)
{
	// peor caso: 6 bytes = 48 bits 
	// mejor caso: 1 bit

	// splitted 1 bit
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits

	splitted=file.Unpack(1)==1;
	if (splitted)
	{
		child[0]=new FLTransQuad3();
		child[0]->Load(file,ranx,rany,ranz,rans>>1);
		child[1]=new FLTransQuad3();
		child[1]->Load(file,ranx+(rans>>1),rany,ranz,rans>>1);
		child[2]=new FLTransQuad3();
		child[2]->Load(file,ranx,rany+(rans>>1),ranz,rans>>1);
		child[3]=new FLTransQuad3();
		child[3]->Load(file,ranx+(rans>>1),rany+(rans>>1),ranz,rans>>1);
		child[4]=new FLTransQuad3();
		child[4]->Load(file,ranx,rany,ranz+(rans>>1),rans>>1);
		child[5]=new FLTransQuad3();
		child[5]->Load(file,ranx+(rans>>1),rany,ranz+(rans>>1),rans>>1);
		child[6]=new FLTransQuad3();
		child[6]->Load(file,ranx,rany+(rans>>1),ranz+(rans>>1),rans>>1);
		child[7]=new FLTransQuad3();
		child[7]->Load(file,ranx+(rans>>1),rany+(rans>>1),ranz+(rans>>1),rans>>1);
	}
	else
	{
		pis=(unsigned char)file.Unpack(5);
		pio=(unsigned char)file.Unpack(7);
		//s=(float)pis/(float)(1<<5)*2.0f-1.0f;
		s=(float)pis*0.0625f-1.0f;
	    //o=(float)pio/(float)((1<<7)-1)*((1.0f+fabs(s))*255.0f);
		o=(float)pio*0.0078740157480314f*(1.0f+fabs(s));
		if (s>0.0) o-=s;
		ranposx=ranx;
		ranposy=rany;
		ranposz=ranz;
		ransize=rans;
		domsize=rans<<1;
		if (pis!=16)
		{
			domposx=(unsigned short)file.Unpack(16);
			domposy=(unsigned short)file.Unpack(16);
			domposz=(unsigned short)file.Unpack(16);
			ori=(unsigned char)file.Unpack(5);
		}	
	}
}