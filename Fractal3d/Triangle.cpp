#include "Triangle.h"
#include "Point.h"
#include "Frustum.h"

Triangle::Triangle(void)
{
	p1=p2=p3=0x0;
	rightc=leftc=rightn=leftn=basen=father=r0=r1=t0=t1=0x0;
	level=0;
	cull=CULL_OUT;
	rbound=0.0f;
}

Triangle::Triangle(Point* p1aux,Point* p2aux,Point* p3aux)
{
	p1=p1aux;
	p2=p2aux;
	p3=p3aux;
	rightc=leftc=rightn=leftn=basen=father=r0=r1=t0=t1=0x0;
	level=0;
	cull=CULL_OUT;
	UpdateRBound();
}

Triangle::~Triangle(void)
{
}

void Triangle::UpdateRBound(void)
{
	center=(p1->v+p2->v+p3->v)*0.33333333333f;

	float max=0.0f;
	float auxx=p2->v.x-p1->v.x;
	float auxy=p2->v.y-p1->v.y;
	float auxz=p2->v.z-p1->v.z;
	float auxsum=auxx*auxx+auxy*auxy+auxz*auxz;
	if (auxsum>max) max=auxsum;
	auxx=p3->v.x-p2->v.x;
	auxy=p3->v.y-p2->v.y;
	auxz=p3->v.z-p2->v.z;
	auxsum=auxx*auxx+auxy*auxy+auxz*auxz;
	if (auxsum>max) max=auxsum;
	auxx=p1->v.x-p3->v.x;
	auxy=p1->v.y-p3->v.y;
	auxz=p1->v.z-p3->v.z;
	auxsum=auxx*auxx+auxy*auxy+auxz*auxz;
	if (auxsum>max) max=auxsum;
	rbound=max;

	auxy=p2->v.y-p1->v.y;
	if (auxy>max) max=auxy;
	auxy=p3->v.y-p2->v.y;
	if (auxy>max) max=auxy;
	auxy=p1->v.y-p3->v.y;
	if (auxy>max) max=auxy;
	rbound+=0.25f*(max*max);
}