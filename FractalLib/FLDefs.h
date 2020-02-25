#ifndef __FLDEFS_H__
#define __FLDEFS_H__

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
	
	#define FRACTALLIB_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#ifdef _MSC_VER
		#pragma warning(disable:4251) // elimina el warning de la interfaz dll para la std
		#ifdef FRACTALLIB_EXPORTS
		#define FRACTALLIB_API __declspec(dllexport)
		#else
		#define FRACTALLIB_API __declspec(dllimport)
		#endif
	#else
		#define FRACTALLIB_API
	#endif

#else
	
	#define FRACTALLIB_LINUX
	#define FRACTALLIB_API

#endif

#endif