#ifndef __FLVECTOR_H__
#define __FLVECTOR_H__

#include "FLDefs.h"
#include <math.h>

class FRACTALLIB_API FLVector2
{

public:

	float x,y;		//!< The x,y coordinates.

	inline FLVector2(const float vx=1.0f,const float vy=1.0f){ x=vx; y=vy; }	
	inline FLVector2 operator+(const FLVector2 &v) const { return FLVector2(x+v.x,y+v.y); }
	inline FLVector2 operator-(const FLVector2 &v) const { return FLVector2(x-v.x,y-v.y); }
	inline FLVector2 operator-(void) const { return FLVector2(-x,-y); }
	inline void operator+=(const FLVector2 &v){ x+=v.x; y+=v.y; }
	inline void operator-=(const FLVector2 &v){ x-=v.x; y-=v.y; }
	inline FLVector2 operator*(const float f) const { return FLVector2(x*f,y*f); }
	inline float operator*(const FLVector2 &v) const { return x*v.x+y*v.y; }
	inline float operator^(const FLVector2 &v){ return x*v.y-y*v.x; }
	inline FLVector2 operator/(float f) const { return (*this)*(1.0f/f); }
	inline FLVector2 operator/=(float f){ x/=f; y/=f; return (*this); }
	inline float operator%(const FLVector2 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const FLVector2 &v) const { return (x==v.x && y==v.y); }
	inline bool operator!=(const FLVector2 &v) const { return (x!=v.x || y!=v.y); }
	inline FLVector2 operator=(const FLVector2 &v){ x=v.x; y=v.y; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const FLVector2 &v) const { return (x>v.x && y>v.y);}
	inline bool operator<(const FLVector2 &v) const { return (x<v.x && y<v.y);}
	inline bool operator>=(const FLVector2 &v) const { return (x>=v.x && y>=v.y);}
	inline bool operator<=(const FLVector2 &v) const { return (x<=v.x && y<=v.y);}
	inline float Module(void) const { return sqrtf(x*x + y*y); }
	inline FLVector2 Normalize(void) const { return (*this)/Module(); }

};

class FLVector3
{

public:

	float x,y,z;		//!< The x,y,z coordinates.

	inline FLVector3(const float vx=1.0f,const float vy=1.0f,const float vz=1.0f){ x=vx; y=vy; z=vz; }	
	inline FLVector3(const FLVector2 &v){ x=v.x;y=v.y;z=1.0f; }
	inline operator FLVector2(void){ return FLVector2(x,y); }
	inline FLVector3 operator+(const FLVector3 &v) const { return FLVector3(x+v.x,y+v.y,z+v.z); }
	inline FLVector3 operator-(const FLVector3 &v) const { return FLVector3(x-v.x,y-v.y,z-v.z); }
	inline FLVector3 operator-(void) const { return FLVector3(-x,-y,-z); }
	inline void operator+=(const FLVector3 &v){ x+=v.x; y+=v.y; z+=v.z; }
	inline void operator-=(const FLVector3 &v){ x-=v.x; y-=v.y; z-=v.z; }
	inline FLVector3 operator*(const float f) const { return FLVector3(x*f,y*f,z*f); }
	inline float operator*(const FLVector3 &v) const { return x*v.x+y*v.y+z*v.z; }
	inline FLVector3 operator^(const FLVector3 &v) const { return FLVector3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
	inline FLVector3 operator/(float f) const { return (*this)*(1.0f/f); }
	inline FLVector3 operator/=(float f){ x/=f; y/=f; z/=f; return (*this); }
	inline float operator%(const FLVector3 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const FLVector3 &v) const { return (x==v.x && y==v.y && z==v.z); }
	inline bool operator!=(const FLVector3 &v) const { return (x!=v.x || y!=v.y || z!=v.z); }
	inline FLVector3 operator=(const FLVector3 &v){ x=v.x; y=v.y; z=v.z; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const FLVector3 &v) const { return (x>v.x && y>v.y && z>v.z);}
	inline bool operator<(const FLVector3 &v) const { return (x<v.x && y<v.y && z<v.z);}
	inline bool operator>=(const FLVector3 &v) const { return (x>=v.x && y>=v.y && z>=v.z);}
	inline bool operator<=(const FLVector3 &v) const { return (x<=v.x && y<=v.y && z<=v.z);}
	inline float Module(void) const { return sqrtf(x*x + y*y + z*z); }
	inline FLVector3 Normalize(void) const { return (*this)/Module(); }

};

class FLVector4
{

public:

	float x,y,z,w;		//!< The x,y,z,w coordinates.

	inline FLVector4(const float vx=1.0f,const float vy=1.0f,const float vz=1.0f,const float vw=1.0f){ x=vx; y=vy; z=vz; w=vw; }	
	inline FLVector4(const FLVector2 &v){ x=v.x;y=v.y;z=1.0f;w=1.0f; }
	inline FLVector4(const FLVector3 &v){ x=v.x;y=v.y;z=v.z;w=1.0f; }
	inline operator FLVector2(void){ return FLVector2(x,y); }
	inline operator FLVector3(void){ return FLVector3(x,y,z); }
	inline FLVector4 operator+(const FLVector4 &v) const { return FLVector4(x+v.x,y+v.y,z+v.z,w+v.w); }
	inline FLVector4 operator-(const FLVector4 &v) const { return FLVector4(x-v.x,y-v.y,z-v.z,w-v.w); }
	inline FLVector4 operator-(void) const { return FLVector4(-x,-y,-z,-w); }
	inline void operator+=(const FLVector4 &v){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; }
	inline void operator-=(const FLVector4 &v){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; }
	inline FLVector4 operator*(const float f) const { return FLVector4(x*f,y*f,z*f,w*f); }
	inline float operator*(const FLVector4 &v) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
	inline FLVector4 operator^(const FLVector4 &v) const { return FLVector4(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
	inline FLVector4 operator/(float f) const { return (*this)*(1.0f/f); }
	inline FLVector4 operator/=(float f){ x/=f; y/=f; z/=f; w/=f; return (*this); }
	inline float operator%(const FLVector4 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const FLVector4 &v) const { return (x==v.x && y==v.y && z==v.z && w==v.w); }
	inline bool operator!=(const FLVector4 &v) const { return (x!=v.x || y!=v.y || z!=v.z || w!=v.w); }
	inline FLVector4 operator=(const FLVector4 &v){ x=v.x; y=v.y; z=v.z; w=v.w; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const FLVector4 &v) const { return (x>v.x && y>v.y && z>v.z && w>v.w);}
	inline bool operator<(const FLVector4 &v) const { return (x<v.x && y<v.y && z<v.z && w<v.w);}
	inline bool operator>=(const FLVector4 &v) const { return (x>=v.x && y>=v.y && z>=v.z && w>=v.w);}
	inline bool operator<=(const FLVector4 &v) const { return (x<=v.x && y<=v.y && z<=v.z && w<=v.w);}
	inline float Module(void) const { return sqrtf(x*x + y*y + z*z + w*w); }
	inline FLVector4 Normalize(void) const { return (*this)/Module(); }

};

#endif