#include "FLTransConst.h"

FLTransConst2::FLTransConst2()
{
}

FLTransConst2::FLTransConst2(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxransize)
:FLTrans2(auxdomposx,auxdomposy,auxdomsize,auxranposx,auxranposy,auxransize)
{
}

FLTransConst2::~FLTransConst2()
{
}

void FLTransConst2::Save(FLFile &file)
{
	// peor caso: 5 bytes + 7 bits = 47 bits 
	// mejor caso: 1 byte + 4 bits = 12 bits
	
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits
	
	file.Pack(5,pis);
	file.Pack(7,pio);
	if (pis!=16)
	{
		file.Pack(16,domposx);
		file.Pack(16,domposy);		
		file.Pack(3,ori); // 8 posibilidades
	}
}

void FLTransConst2::Load(FLFile &file)
{
	// peor caso: 5 bytes + 7 bits = 47 bits 
	// mejor caso: 1 byte + 4 bits = 12 bits

	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// ori 3 bits	
	
	pis=(unsigned char)file.Unpack(5);
	pio=(unsigned char)file.Unpack(7);
	if (pis!=16)
	{
		domposx=(unsigned short)file.Unpack(16);
		domposy=(unsigned short)file.Unpack(16);
		ori=(unsigned char)file.Unpack(3);	
	}	
}

FLTransConst3::FLTransConst3()
{
}

FLTransConst3::FLTransConst3(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomposz, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxranposz, unsigned short auxransize)
:FLTrans3(auxdomposx,auxdomposy,auxdomposz,auxdomsize,auxranposx,auxranposy,auxranposz,auxransize)
{
}

FLTransConst3::~FLTransConst3()
{
}

void FLTransConst3::Save(FLFile &file)
{
	// peor caso: ?
	// mejor caso: ?
	
	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// domposz 2 bytes
	// ori 5 bits
	
	file.Pack(5,pis);
	file.Pack(7,pio);
	if (pis!=16)
	{
		file.Pack(16,domposx);
		file.Pack(16,domposy);
		file.Pack(16,domposz);
		file.Pack(5,ori); // 32 posibilidades
	}
}

void FLTransConst3::Load(FLFile &file)
{
	// peor caso: ?
	// mejor caso: ?

	// s 5 bits
	// o 7 bits
	// domposx 2 bytes
	// domposy 2 bytes
	// domposz 2 bytes
	// ori 5 bits	
	
	pis=(unsigned char)file.Unpack(5);
	pio=(unsigned char)file.Unpack(7);
	if (pis!=16)
	{
		domposx=(unsigned short)file.Unpack(16);
		domposy=(unsigned short)file.Unpack(16);
		domposz=(unsigned short)file.Unpack(16);
		ori=(unsigned char)file.Unpack(5);	
	}	
}