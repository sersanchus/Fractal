#if defined(_MSC_VER)
	#if (_MSC_VER >= 1400)
		#define _CRT_SECURE_NO_DEPRECATE
	#endif
	#include <windows.h>
	#include "../FractalLib/FLBitmap.h"
	#include "../FractalLib/FLFractal.h"
	#include "../FractalLib/FLFractalConst.h"
	#include "../FractalLib/FLFractalQuad.h"
	#include "../FractalLib/FLTrans.h"
	#include <io.h>
	#define SplitPath(fu,dr,di,fi,e) _splitpath(fu,dr,di,fi,e)
#elif defined(__CYGWIN__) || defined(__MINGW32__)
	#include <windows.h>
	#include "FLBitmap.h"
	#include "FLFractal.h"
	#include "FLFractalConst.h"
	#include "FLFractalQuad.h"
	#include "FLTrans.h"
	#include <io.h>
	#define SplitPath(fu,dr,di,fi,e) _splitpath(fu,dr,di,fi,e)
#else
	#include <sys/time.h>
	#include "FLBitmap.h"
	#include "FLFractal.h"
	#include "FLFractalConst.h"
	#include "FLFractalQuad.h"
	#include "FLTrans.h"
	#define SplitPath(fu,dr,di,fi,e) fnsplit(fu,dr,di,fi,e)
#endif
#include <fstream>
//#include <GL/glut.h>
#include <iostream>

using namespace std;


//YIQ
#define conversion "yiq"
#define RTOX 0.299f
#define RTOY 0.596f
#define RTOZ 0.212f
#define GTOX 0.587f
#define GTOY -0.274f
#define GTOZ -0.523f
#define BTOX 0.114f
#define BTOY -0.322f
#define BTOZ 0.311f


//YUV
/*#define conversion "yuv"
#define RTOX 0.299f
#define RTOY -0.147f
#define RTOZ 0.615f
#define GTOX 0.587f
#define GTOY -0.289f
#define GTOZ -0.515f
#define BTOX 0.114f
#define BTOY 0.436f
#define BTOZ -0.1f*/

unsigned short tamxentradaorig=256;
unsigned short tamyentradaorig=256;
unsigned short tamzentradaorig=0;
unsigned short tamxentrada=256;
unsigned short tamyentrada=256;
unsigned short tamzentrada=0;
unsigned char bytespp=1;
unsigned short tamanyorango=4;
unsigned int numerotransmax=5000;
unsigned char tipofractal=FRACTAL_QUAD;
unsigned short saltoendominios=4;
unsigned char dimensionesfractal=FRACTAL_2D;
char ficheroacodificar[256];
char ficherofractal[256];

long tiempo;
FLFile *file;
unsigned char version=1;
void* fractal;
void* finbit;
FLBitmap2 *finbitaux1,*finbitaux2;

long Ticks()
{
#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	return GetTickCount();
#else
	timeval tv;
	__timezone_ptr_t tz;
	gettimeofday(&tv,tz);
	long res=tv.tv_sec*1000L+tv.tv_usec/1000L;
	return res;	
#endif
}

void MuestraTiempo(void)
{
	cout << endl;
	tiempo=Ticks()-tiempo;
	/*long horas=tiempo/3600000;
	tiempo-=3600000*horas;
	long minutos=tiempo/60000;
	tiempo-=60000*minutos;
	long segundos=tiempo/1000;
	tiempo-=1000*segundos;*/
	cout << "time:\t";
	/*if (horas) cout << horas << " h ";
	if (minutos) cout << minutos << " m ";
	if (segundos) cout << segundos << " s ";*/
	if (tiempo) cout << tiempo << " ms";
	cout << endl;
}

void MuestraDiferencia(void)
{
	// recargamos el fractal tal y como lo hará el fractalview
	file->Close();
	file->Open(ficherofractal,"r+b");
	char clave[8];
	clave[0]=(char)file->Unpack(8);
	clave[1]=(char)file->Unpack(8);
	clave[2]=(char)file->Unpack(8);
	clave[3]=(char)file->Unpack(8);
	clave[4]=(char)file->Unpack(8);
	clave[5]=(char)file->Unpack(8);
	clave[6]=(char)file->Unpack(8);
	clave[7]='\0';
	version=(unsigned char)file->Unpack(8);
	unsigned char type=(unsigned char)file->Unpack(8);
	tamxentrada=(unsigned short)file->Unpack(16);
	tamyentrada=(unsigned short)file->Unpack(16);
	if (type==FRACTAL_3D)
		tamzentrada=(unsigned short)file->Unpack(16);
	bool color=(file->Unpack(1)==1);
	
	if (type==FRACTAL_2D)
	{
		FLFractal2* fractal2=(FLFractal2*)fractal;
		FLBitmap2* finbitaux=(FLBitmap2*)finbit;
		if (color)
		{
			float med,max,mse,psnr;
			med=max=mse=psnr=0.0f;
			for (unsigned int i=0;i<3;i++)
			{
				tipofractal=(unsigned char)file->Unpack(8);

				fractal2->imagetamx=(i==0)?tamxentrada:tamxentrada/2;
				fractal2->imagetamy=(i==0)?tamyentrada:tamyentrada/2;
				fractal2->Load(*file);

				unsigned int veces=0;
				FLBitmap2 finbit2(fractal2->imagetamx,fractal2->imagetamy);
				FLBitmap2 finbit3(fractal2->imagetamx,fractal2->imagetamy);
				float dif;
				do
				{
					finbit3=fractal2->Decode(finbit2);
					dif=finbit2.DiferenceMean(finbit3);
					finbit2=finbit3;
					veces++;
				}while(dif>0.0000001f);
								
				med+=finbit2.DiferenceMean(*finbitaux);
				if (finbit2.DiferenceMax(*finbitaux)>max) max=finbit2.DiferenceMax(*finbitaux);
				mse+=finbit2.DiferenceMse(*finbitaux);
				//psnr+=finbit2.DiferencePsnr(*finbitaux);

				if (i==0)
					finbitaux=finbitaux1;
				else if (i==1)
					finbitaux=finbitaux2;
			}
			med/=3.0f;
			med*=255.0f;
			max*=255.0f;
			mse/=3.0f;
			psnr=20.0f*log10f(1.0f/sqrtf(mse));
			mse*=255.0f;
			
			cout << endl;
			cout << "mean:\t" << med << " pixels" << endl;
			cout << "max:\t" << max << " pixels" << endl;
			cout << "mse:\t" << mse << " pixels" << endl;
			cout << "psnr:\t" << psnr << " db" << endl;
		}
		else
		{
			tipofractal=(unsigned char)file->Unpack(8);

			fractal2->imagetamx=tamxentrada;
			fractal2->imagetamy=tamyentrada;
			fractal2->Load(*file);

			unsigned int veces=0;
			FLBitmap2 finbit2(finbitaux->tamx,finbitaux->tamy);
			FLBitmap2 finbit3(finbitaux->tamx,finbitaux->tamy);
			float dif;
			do
			{
				finbit3=fractal2->Decode(finbit2);
				dif=finbit2.DiferenceMean(finbit3);
				finbit2=finbit3;
				veces++;
			}while(dif>0.0000001f);

			cout << endl;
			cout << "mean:\t" << finbit2.DiferenceMean(*finbitaux)*255.0f << " pixels" << endl;
			cout << "max:\t" << finbit2.DiferenceMax(*finbitaux)*255.0f << " pixels" << endl;
			cout << "mse:\t" << finbit2.DiferenceMse(*finbitaux)*255.0f << " pixels" << endl;
			cout << "psnr:\t" << finbit2.DiferencePsnr(*finbitaux) << " db" << endl;
		}
	}
	else
	{
		FLFractal3* fractal3=(FLFractal3*)fractal;
		
		tipofractal=(unsigned char)file->Unpack(8);
		
		fractal3->imagetamx=tamxentrada;
		fractal3->imagetamy=tamyentrada;
		fractal3->imagetamz=tamzentrada;
		fractal3->Load(*file);

		FLBitmap3* finbitaux=(FLBitmap3*)finbit;

		unsigned int veces=0;
		FLBitmap3 finbit2(finbitaux->tamx,finbitaux->tamy,finbitaux->tamz);
		FLBitmap3 finbit3(finbitaux->tamx,finbitaux->tamy,finbitaux->tamz);
		float dif;
		do
		{
			finbit3=fractal3->Decode(finbit2);
			dif=finbit2.DiferenceMean(finbit3);
			finbit2=finbit3;
			veces++;
		}while(dif>0.0000001f);

		cout << endl;
		cout << "mean:\t" << finbit2.DiferenceMean(*finbitaux)*255.0f << " pixels" << endl;
		cout << "max:\t" << finbit2.DiferenceMax(*finbitaux)*255.0f << " pixels" << endl;
		cout << "mse:\t" << finbit2.DiferenceMse(*finbitaux)*255.0f << " pixels" << endl;
		cout << "psnr:\t" << finbit2.DiferencePsnr(*finbitaux) << " db" << endl;
	}	
}

int main(int argc,char **argv)
{
	if (argc>1)
	{
		if (argc!=7)
		{
			cout << "Use mode: raw [width]x[height]x[deep] bytespp jump fractaltype fractalparam" << endl;
			exit(1);
		}
		strncpy(ficheroacodificar,argv[1],256);
		char drive[255];
		char dir[255];
		char file[255];
		char ext[255];
		SplitPath(argv[1],drive,dir,file,ext);
				
		// leo el formato width x height x deep
		int posfind=0;
		int posstring=0;
		int itam=0;
		string tams[3];
		string straux=argv[2];		
		posfind=(int)straux.find("x",posstring);
		while(posfind!=-1)
		{
			tams[itam]=straux.substr(posstring,posfind-posstring);
			posstring=posfind+1;
			posfind=(int)straux.find("x",posstring);
			itam++;
		}
		tams[itam]=straux.substr(posstring,straux.length()-posstring);
		tamxentrada=tamxentradaorig=atoi(tams[0].c_str());
		tamyentrada=tamyentradaorig=atoi(tams[1].c_str());
		tamzentrada=tamzentradaorig=atoi(tams[2].c_str());

		if (itam==1)
		{
			dimensionesfractal=FRACTAL_2D;
			sprintf(ficherofractal,"%s%s%s.fra2",drive,dir,file);
		}
		else
		{
			dimensionesfractal=FRACTAL_3D;
			sprintf(ficherofractal,"%s%s%s.fra3",drive,dir,file);
		}
		
		bytespp=atoi(argv[3]);
		saltoendominios=atoi(argv[4]);
		if (strcmp(argv[5],"quad")==0)
			tipofractal=FRACTAL_QUAD;
		else if (strcmp(argv[5],"const")==0)
			tipofractal=FRACTAL_CONST;
		else
        	tipofractal=atoi(argv[5]);
		if (tipofractal==FRACTAL_QUAD)
			numerotransmax=atoi(argv[6]);
		else if (tipofractal==FRACTAL_CONST)
			tamanyorango=atoi(argv[6]);
	}
	else
	{
		tamxentrada=tamxentradaorig=256;
		tamyentrada=tamyentradaorig=256;
		tamzentrada=tamzentradaorig=0;
		bytespp=1;
		tamanyorango=4;
		numerotransmax=5000;
		tipofractal=FRACTAL_QUAD;
		dimensionesfractal=FRACTAL_2D;
		saltoendominios=4;
		strcpy(ficheroacodificar,"test.raw");
		strcpy(ficherofractal,"test.fra2");
	}

	#if defined(_WIN32) || defined(__CYGWIN__) || defined(_MINGW_)
	if (_access(ficheroacodificar,0)==-1)
	#else
	if (access(ficheroacodificar,0)==-1)
	#endif
	{
		cout << ficheroacodificar << " doesn't exist" << endl;
		exit(1);
	}
	
	ifstream fin(ficheroacodificar,std::ios_base::binary);

	file=new FLFile(ficherofractal,"w+b");
	file->Pack(8,'F');
	file->Pack(8,'r');
	file->Pack(8,'a');
	file->Pack(8,'c');
	file->Pack(8,'t');
	file->Pack(8,'a');
	file->Pack(8,'l');
	file->Pack(8,version);
	file->Pack(8,dimensionesfractal);

	cout << "file:\t" << ficherofractal << endl;
	cout << "key:\t" << "Fractal" << endl;
	cout << "rev:\t" << (int)version << endl;

	if (dimensionesfractal==FRACTAL_2D)
	{
		if (tipofractal==FRACTAL_QUAD)
		{
			fractal=new FLFractalQuad2();
			tamxentrada=(unsigned short)(ceilf((float)tamxentrada/32.0f)*32.0f);
			tamyentrada=(unsigned short)(ceilf((float)tamyentrada/32.0f)*32.0f);
		}
		else if (tipofractal==FRACTAL_CONST)
		{
			fractal=new FLFractalConst2();
			tamxentrada=(unsigned short)(ceilf((float)tamxentrada/(float)(tamanyorango<<2))*(float)(tamanyorango<<2));
			tamyentrada=(unsigned short)(ceilf((float)tamyentrada/(float)(tamanyorango<<2))*(float)(tamanyorango<<2));
		}

		cout << "type:\t2d" << endl;		
		cout << "width:\t" << tamxentrada << endl;
		cout << "height:\t" << tamyentrada << endl;
		cout << "color:\t";
		if (bytespp==3)
			cout << "rgb -> " << conversion << endl;
		else
			cout << "grayscale" << endl;
		cout << endl;

		file->Pack(16,tamxentrada);
		file->Pack(16,tamyentrada);
		if (bytespp==3)
			file->Pack(1,FRACTAL_TRUECOLOR);
		else
			file->Pack(1,FRACTAL_GRAYSCALE);

		finbit=new FLBitmap2(tamxentrada,tamyentrada);
		FLBitmap2* finbit2=(FLBitmap2*)finbit;
		FLFractal2* fractal2=(FLFractal2*)fractal;
			
		// empieza compresión
		tiempo=Ticks();
		if (bytespp==3)
		{
			float coloraux;
			cout << "encoding channel 0... " << endl;
			for (unsigned int j=0;j<tamyentrada;j++)
			{
				for (unsigned int i=0;i<tamxentrada;i++)
				{
					if (j<tamyentradaorig && i<tamxentradaorig)
						coloraux=RTOX*(float)fin.get()+GTOX*(float)fin.get()+BTOX*(float)fin.get();
					else
						coloraux=RTOX*255.0f+GTOX*255.0f+BTOX*255.0f;

					coloraux*=0.003921568627f;

					finbit2->Pixel(i,j)=coloraux;
				}
			}
			if (tipofractal==FRACTAL_CONST)
				((FLFractalConst2*)fractal)->Encode(finbit2,saltoendominios,tamanyorango);
			else if (tipofractal==FRACTAL_QUAD)
				((FLFractalQuad2*)fractal)->Encode(finbit2,saltoendominios,numerotransmax);
			cout << "saving fractal..." << endl << endl;
			file->Pack(8,tipofractal);
			fractal2->Save(*file);
			fin.clear();
			fin.seekg(0);

			FLBitmap2 parareducirlo(tamxentrada,tamyentrada);

			finbitaux1=new FLBitmap2(finbit2->tamx/2,finbit2->tamy/2);
			cout << "encoding channel 1... " << endl;
			for (unsigned int j=0;j<tamyentrada;j++)
			{
				for (unsigned int i=0;i<tamxentrada;i++)
				{
					if (j<tamyentradaorig && i<tamxentradaorig)
						coloraux=RTOY*(float)fin.get()+GTOY*(float)fin.get()+BTOY*(float)fin.get();
					else
						coloraux=RTOY*255.0f+GTOY*255.0f+BTOY*255.0f;

					coloraux*=0.003921568627f;

					parareducirlo.Pixel(i,j)=coloraux;
				}
			}
			parareducirlo.SubSample2(*finbitaux1);
			if (tipofractal==FRACTAL_CONST)
				((FLFractalConst2*)fractal)->Encode(finbitaux1,saltoendominios,tamanyorango<<1);
			else if (tipofractal==FRACTAL_QUAD)
				((FLFractalQuad2*)fractal)->Encode(finbitaux1,saltoendominios,numerotransmax>>1);
			cout << "saving fractal..." << endl << endl;
			file->Pack(8,tipofractal);
			fractal2->Save(*file);
			fin.clear();
			fin.seekg(0);
			
			finbitaux2=new FLBitmap2(finbit2->tamx/2,finbit2->tamy/2);
			cout << "encoding channel 2... " << endl;
			for (unsigned int j=0;j<tamyentrada;j++)
			{
				for (unsigned int i=0;i<tamxentrada;i++)
				{
					if (j<tamyentradaorig && i<tamxentradaorig)
						coloraux=RTOZ*(float)fin.get()+GTOZ*(float)fin.get()+BTOZ*(float)fin.get();
					else
						coloraux=RTOZ*255.0f+GTOZ*255.0f+BTOZ*255.0f;

					coloraux*=0.003921568627f;

					parareducirlo.Pixel(i,j)=coloraux;
				}
			}
			parareducirlo.SubSample2(*finbitaux2);
			if (tipofractal==FRACTAL_CONST)
				((FLFractalConst2*)fractal)->Encode(finbitaux2,saltoendominios,tamanyorango<<1);
			else if (tipofractal==FRACTAL_QUAD)
				((FLFractalQuad2*)fractal)->Encode(finbitaux2,saltoendominios,numerotransmax>>1);
			cout << "saving fractal..." << endl;
			file->Pack(8,tipofractal);
			fractal2->Save(*file);

			// termina compresión
			MuestraTiempo();
			MuestraDiferencia();

			delete finbitaux1;
			delete finbitaux2;
		}
		else if (bytespp==1)
		{
			file->Pack(8,tipofractal);
			cout << "encoding grayscale channel... " << endl;
			for (unsigned int j=0;j<tamyentrada;j++)
				for (unsigned int i=0;i<tamxentrada;i++)
					finbit2->Pixel(i,j)=((float)fin.get())*0.003921568627f;
			if (tipofractal==FRACTAL_CONST)
				((FLFractalConst2*)fractal)->Encode(finbit2,saltoendominios,tamanyorango);
			else if (tipofractal==FRACTAL_QUAD)
				((FLFractalQuad2*)fractal)->Encode(finbit2,saltoendominios,numerotransmax);
			cout << "saving fractal..." << endl;
			fractal2->Save(*file);

			// termina compresión
			MuestraTiempo();
			MuestraDiferencia();
		}
		

		delete finbit2;
		delete fractal2;
	}
	else // FRACTAL_3D
	{
		if (tipofractal==FRACTAL_QUAD)
		{
			fractal=new FLFractalQuad3();
			/*tamxentrada=(unsigned short)(ceilf((float)tamxentrada/32.0f)*32.0f);
			tamyentrada=(unsigned short)(ceilf((float)tamyentrada/32.0f)*32.0f);
			tamzentrada=(unsigned short)(ceilf((float)tamzentrada/32.0f)*32.0f);*/
		}
		else if (tipofractal==FRACTAL_CONST)
		{
			fractal=new FLFractalConst3();
			/*tamxentrada=(unsigned short)(ceilf((float)tamxentrada/(float)(tamanyorango<<2))*(float)(tamanyorango<<2));
			tamyentrada=(unsigned short)(ceilf((float)tamyentrada/(float)(tamanyorango<<2))*(float)(tamanyorango<<2));
			tamzentrada=(unsigned short)(ceilf((float)tamzentrada/(float)(tamanyorango<<2))*(float)(tamanyorango<<2));*/
		}

		cout << "type:\t3d" << endl;
		cout << "width:\t" << tamxentrada << endl;
		cout << "height:\t" << tamyentrada << endl;
		cout << "deep:\t" << tamzentrada << endl;
		cout << "color:\t";
		if (bytespp==3)
			cout << "rgb -> " << conversion << endl;
		else
			cout << "grayscale" << endl;
		cout << endl;

		file->Pack(16,tamxentrada);
		file->Pack(16,tamyentrada);
		file->Pack(16,tamzentrada);
		if (bytespp==3)
			file->Pack(1,FRACTAL_TRUECOLOR);
		else
			file->Pack(1,FRACTAL_GRAYSCALE);
		file->Pack(8,tipofractal);

		finbit=new FLBitmap3(tamxentrada,tamyentrada,tamzentrada);
		FLBitmap3* finbit3=(FLBitmap3*)finbit;
		FLFractal3* fractal3=(FLFractal3*)fractal;

		tiempo=Ticks();
		if (bytespp==3)
		{
		}
		else
		{
			cout << "encoding grayscale channel... " << endl;
			for (unsigned int k=0;k<tamzentrada;k++)
				for (unsigned int j=0;j<tamyentrada;j++)
					for (unsigned int i=0;i<tamxentrada;i++)
						finbit3->Pixel(i,j,k)=((float)fin.get())*0.003921568627f;
			if (tipofractal==FRACTAL_CONST)
				((FLFractalConst3*)fractal)->Encode(finbit3,saltoendominios,tamanyorango);
			else if (tipofractal==FRACTAL_QUAD)
				((FLFractalQuad3*)fractal)->Encode(finbit3,saltoendominios,numerotransmax);
			cout << "saving fractal..." << endl;
			fractal3->Save(*file);
		}
		// termina compresión
		MuestraTiempo();
		MuestraDiferencia();

		delete finbit3;
		delete fractal3;
	}	
			
	// muestra ratio de compresión
	ifstream fout(ficherofractal,std::ios_base::binary);
	fout.seekg(0,std::ios_base::end);
	fout.clear();
	fin.clear();
	float sizeraw=(float)fin.tellg();
	float sizefractal=(float)fout.tellg();
	cout << endl;
	cout << "ratio:\t" << sizeraw/sizefractal << ":1" << endl;
	fout.close();
	
	fin.close();
	file->Close();

	return 0;
}