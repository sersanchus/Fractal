#ifndef __POINT_H__
#define __POINT_H__

#include "Vector.h"

class Triangle;

class Point
{

public:

	Triangle* trianglefather;

	Vector3 v;
	Vector3 n;
	//Vector3 c;
	Vector2 t;

	float quality;

	Point *p0,*p1; // lista de libres/puntos a refinar

	Point(void);
	Point(const Vector3&);
	Point(const Point&);
	Point(float,float,float);

	Point operator=(const Point&);

};

#endif
