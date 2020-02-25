#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "Vector.h"

class Point;

class Triangle
{

public:

	Point *p1,*p2,*p3;

	Vector3 center;
		
	Triangle *rightc;
	Triangle *leftc;
	Triangle *rightn;
	Triangle *leftn;
	Triangle *basen;
	Triangle *father;

	Triangle *t0,*t1; // lista de libres/pintados
	Triangle *r0,*r1; // lista de prioridades

	unsigned char cull;

	unsigned char level;

	float rbound;

	float metric;
	float realmetric;

	Triangle(void);
	Triangle(Point*,Point*,Point*);
	~Triangle(void);

	void UpdateRBound(void);

};

#endif