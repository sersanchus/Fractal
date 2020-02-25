#include "FLTrans.h"
#include <math.h>

FLTrans2::FLTrans2()
{
}

FLTrans2::FLTrans2(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxransize)
{
	domposx=auxdomposx;
	domposy=auxdomposy;
	domsize=auxdomsize;
	ranposx=auxranposx;
	ranposy=auxranposy;
	ransize=auxransize;
}

FLTrans2::~FLTrans2()
{
}

FLTrans2& FLTrans2::operator=(const FLTrans2 &t)
{
	domposx=t.domposx;
	domposy=t.domposy;
	domsize=t.domsize;
	ranposx=t.ranposx;
	ranposy=t.ranposy;
	ransize=t.ransize;
	ori=t.ori;
	s=t.s;
	pis=t.pis;
	o=t.o;
	pio=t.pio;
	rms=t.rms;
	//dif=t.dif;
	return *this;
}

FLTrans3::FLTrans3()
{
}

FLTrans3::FLTrans3(unsigned short auxdomposx, unsigned short auxdomposy, unsigned short auxdomposz, unsigned short auxdomsize,unsigned short auxranposx, unsigned short auxranposy, unsigned short auxranposz, unsigned short auxransize)
{
	domposx=auxdomposx;
	domposy=auxdomposy;
	domposz=auxdomposz;
	domsize=auxdomsize;
	ranposx=auxranposx;
	ranposy=auxranposy;
	ranposz=auxranposz;
	ransize=auxransize;
}

FLTrans3::~FLTrans3()
{
}

FLTrans3& FLTrans3::operator=(const FLTrans3 &t)
{
	domposx=t.domposx;
	domposy=t.domposy;
	domposz=t.domposz;
	domsize=t.domsize;
	ranposx=t.ranposx;
	ranposy=t.ranposy;
	ranposz=t.ranposz;
	ransize=t.ransize;
	ori=t.ori;
	s=t.s;
	pis=t.pis;
	o=t.o;
	pio=t.pio;
	rms=t.rms;
	//dif=t.dif;
	return *this;
}