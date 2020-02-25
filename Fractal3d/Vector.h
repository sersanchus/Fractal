#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>

class Vector2
{

public:

	float x,y;		//!< The x,y coordinates.

	inline Vector2(const float vx=1.0f,const float vy=1.0f){ x=vx; y=vy; }	
	inline Vector2 operator+(const Vector2 &v) const { return Vector2(x+v.x,y+v.y); }
	inline Vector2 operator-(const Vector2 &v) const { return Vector2(x-v.x,y-v.y); }
	inline Vector2 operator-(void) const { return Vector2(-x,-y); }
	inline void operator+=(const Vector2 &v){ x+=v.x; y+=v.y; }
	inline void operator-=(const Vector2 &v){ x-=v.x; y-=v.y; }
	inline Vector2 operator*(const float f) const { return Vector2(x*f,y*f); }
	inline float operator*(const Vector2 &v) const { return x*v.x+y*v.y; }
	inline float operator^(const Vector2 &v){ return x*v.y-y*v.x; }
	inline Vector2 operator/(float f) const { return (*this)*(1.0f/f); }
	inline Vector2 operator/=(float f){ x/=f; y/=f; return (*this); }
	inline float operator%(const Vector2 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const Vector2 &v) const { return (x==v.x && y==v.y); }
	inline bool operator!=(const Vector2 &v) const { return (x!=v.x || y!=v.y); }
	inline Vector2 operator=(const Vector2 &v){ x=v.x; y=v.y; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const Vector2 &v) const { return (x>v.x && y>v.y);}
	inline bool operator<(const Vector2 &v) const { return (x<v.x && y<v.y);}
	inline bool operator>=(const Vector2 &v) const { return (x>=v.x && y>=v.y);}
	inline bool operator<=(const Vector2 &v) const { return (x<=v.x && y<=v.y);}
	inline float Module(void) const { return sqrtf(x*x + y*y); }
	inline Vector2 Normalize(void) const { return (*this)/Module(); }

};

class Vector3
{

public:

	float x,y,z;		//!< The x,y,z coordinates.

	inline Vector3(const float vx=1.0f,const float vy=1.0f,const float vz=1.0f){ x=vx; y=vy; z=vz; }	
	inline Vector3(const Vector2 &v){ x=v.x;y=v.y;z=1.0f; }
	inline operator Vector2(void){ return Vector2(x,y); }
	inline Vector3 operator+(const Vector3 &v) const { return Vector3(x+v.x,y+v.y,z+v.z); }
	inline Vector3 operator-(const Vector3 &v) const { return Vector3(x-v.x,y-v.y,z-v.z); }
	inline Vector3 operator-(void) const { return Vector3(-x,-y,-z); }
	inline void operator+=(const Vector3 &v){ x+=v.x; y+=v.y; z+=v.z; }
	inline void operator-=(const Vector3 &v){ x-=v.x; y-=v.y; z-=v.z; }
	inline Vector3 operator*(const float f) const { return Vector3(x*f,y*f,z*f); }
	inline float operator*(const Vector3 &v) const { return x*v.x+y*v.y+z*v.z; }
	inline Vector3 operator^(const Vector3 &v) const { return Vector3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
	inline Vector3 operator/(float f) const { return (*this)*(1.0f/f); }
	inline Vector3 operator/=(float f){ x/=f; y/=f; z/=f; return (*this); }
	inline float operator%(const Vector3 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const Vector3 &v) const { return (x==v.x && y==v.y && z==v.z); }
	inline bool operator!=(const Vector3 &v) const { return (x!=v.x || y!=v.y || z!=v.z); }
	inline Vector3 operator=(const Vector3 &v){ x=v.x; y=v.y; z=v.z; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const Vector3 &v) const { return (x>v.x && y>v.y && z>v.z);}
	inline bool operator<(const Vector3 &v) const { return (x<v.x && y<v.y && z<v.z);}
	inline bool operator>=(const Vector3 &v) const { return (x>=v.x && y>=v.y && z>=v.z);}
	inline bool operator<=(const Vector3 &v) const { return (x<=v.x && y<=v.y && z<=v.z);}
	inline float Module(void) const { return sqrtf(x*x + y*y + z*z); }
	inline Vector3 Normalize(void) const { return (*this)/Module(); }

};

class Vector4
{

public:

	float x,y,z,w;		//!< The x,y,z,w coordinates.

	inline Vector4(const float vx=1.0f,const float vy=1.0f,const float vz=1.0f,const float vw=1.0f){ x=vx; y=vy; z=vz; w=vw; }	
	inline Vector4(const Vector2 &v){ x=v.x;y=v.y;z=1.0f;w=1.0f; }
	inline Vector4(const Vector3 &v){ x=v.x;y=v.y;z=v.z;w=1.0f; }
	inline operator Vector2(void){ return Vector2(x,y); }
	inline operator Vector3(void){ return Vector3(x,y,z); }
	inline Vector4 operator+(const Vector4 &v) const { return Vector4(x+v.x,y+v.y,z+v.z,w+v.w); }
	inline Vector4 operator-(const Vector4 &v) const { return Vector4(x-v.x,y-v.y,z-v.z,w-v.w); }
	inline Vector4 operator-(void) const { return Vector4(-x,-y,-z,-w); }
	inline void operator+=(const Vector4 &v){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; }
	inline void operator-=(const Vector4 &v){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; }
	inline Vector4 operator*(const float f) const { return Vector4(x*f,y*f,z*f,w*f); }
	inline float operator*(const Vector4 &v) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
	inline Vector4 operator^(const Vector4 &v) const { return Vector4(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x); }
	inline Vector4 operator/(float f) const { return (*this)*(1.0f/f); }
	inline Vector4 operator/=(float f){ x/=f; y/=f; z/=f; w/=f; return (*this); }
	inline float operator%(const Vector4 &v) const { return (*this)*v/(Module()*v.Module()); }
	inline bool operator==(const Vector4 &v) const { return (x==v.x && y==v.y && z==v.z && w==v.w); }
	inline bool operator!=(const Vector4 &v) const { return (x!=v.x || y!=v.y || z!=v.z || w!=v.w); }
	inline Vector4 operator=(const Vector4 &v){ x=v.x; y=v.y; z=v.z; w=v.w; return v; }
	inline float &operator[](const unsigned int i) { return (&x)[i]; }
	inline bool operator>(const Vector4 &v) const { return (x>v.x && y>v.y && z>v.z && w>v.w);}
	inline bool operator<(const Vector4 &v) const { return (x<v.x && y<v.y && z<v.z && w<v.w);}
	inline bool operator>=(const Vector4 &v) const { return (x>=v.x && y>=v.y && z>=v.z && w>=v.w);}
	inline bool operator<=(const Vector4 &v) const { return (x<=v.x && y<=v.y && z<=v.z && w<=v.w);}
	inline float Module(void) const { return sqrtf(x*x + y*y + z*z + w*w); }
	inline Vector4 Normalize(void) const { return (*this)/Module(); }

};

#endif