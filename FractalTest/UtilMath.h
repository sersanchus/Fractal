#ifndef __UTILMATH_H__
#define __UTILMATH_H__

#include "Vector.h"

struct Triangle
{
	Vector3 v[3];
	Vector3 fn;
	Vector3 n[3];
	Vector3 na[3];

	Triangle(){;}
};

struct TriangleNode
{
	Triangle* t;
	TriangleNode* next;

	TriangleNode(){t=0x0;next=0x0;}
};

bool BoxTriangleIntersect(Vector3*,Triangle*);
float DistancePointTriangle(const Vector3&,Triangle*);

#endif