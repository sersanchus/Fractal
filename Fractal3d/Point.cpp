#include "Point.h"

Point::Point(void)
{
	v=Vector3(0.0f,0.0f,0.0f);
	p0=p1=0x0;
	quality=0.0f;
	n=Vector3(0.0f,1.0f,0.0f);
}

Point::Point(const Vector3 &vaux)
{
	v.x=vaux.x;
	v.y=vaux.y;
	v.z=vaux.z;
	p0=p1=0x0;
	quality=0.0f;
	n=Vector3(0.0f,1.0f,0.0f);
}

Point::Point(const Point& paux)
{
	*this=paux;
	n=Vector3(0.0f,1.0f,0.0f);
}

Point::Point(float f1,float f2,float f3)
{
	v.x=f1;
	v.y=f2;
	v.z=f3;
	p0=p1=0x0;
	quality=0.0f;
	n=Vector3(0.0f,1.0f,0.0f);
}

Point Point::operator=(const Point& p)
{
	v.x=p.v.x;
	v.y=p.v.y;
	v.z=p.v.z;
	quality=p.quality;
	p0=p.p0;
	p1=p.p1;
	return p;
}