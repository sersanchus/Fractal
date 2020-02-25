#include "UtilMath.h"

/*float getmin(Vector3* v, unsigned int numvertices, const Vector3& axis)
{
	float min=v[0].x*axis.x+v[0].y*axis.y+v[0].z*axis.z;
	float aux;
	for (unsigned int i=1;i<numvertices;i++)
	{
		aux=v[i].x*axis.x+v[i].y*axis.y+v[i].z*axis.z;
		if (aux<min) min=aux;
	}
	return min;
}

float getmax(Vector3* v, unsigned int numvertices, const Vector3& axis)
{
	float max=v[0].x*axis.x+v[0].y*axis.y+v[0].z*axis.z;
	float aux;
	for (unsigned int i=1;i<numvertices;i++)
	{
		aux=v[i].x*axis.x+v[i].y*axis.y+v[i].z*axis.z;
		if (aux>max) max=aux;
	}
	return max;
}

bool isect(Vector3* v1, unsigned int numvertices1, Vector3* v2, unsigned int numvertices2, const Vector3& axis)
{
	if (getmin(v1, numvertices1, axis) > getmax(v2, numvertices2, axis)) return false;
	if (getmax(v1, numvertices1, axis) < getmin(v2, numvertices2, axis)) return false;
	return true;    
}

bool BoxTriangleIntersect(Vector3* boxpoints,Triangle* t)
{
	// test axes
	if (!isect(boxpoints, 8, t->v, 3, Vector3(1.0f,0.0f,0.0f))) return false;
	if (!isect(boxpoints, 8, t->v, 3, Vector3(0.0f,1.0f,0.0f))) return false;
	if (!isect(boxpoints, 8, t->v, 3, Vector3(0.0f,0.0f,1.0f))) return false;

	// test triangle normal
	Vector3 triedge1=t->v[1]-t->v[0];
	Vector3 triedge2=t->v[2]-t->v[0];
	Vector3 trinormal=triedge1^triedge2;
	if (!isect(boxpoints, 8, t->v, 3, trinormal)) return false;

	// test the 9 edge cross products
	Vector3 triedge3(t->v[0]^t->v[2]);
	Vector3 boxedge1(1.0f,0.0f,0.0f);
	Vector3 boxedge2(0.0f,1.0f,0.0f);
	Vector3 boxedge3(0.0f,0.0f,1.0f);
	if (!isect(boxpoints, 8, t->v, 3, boxedge1^triedge1)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge1^triedge2)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge1^triedge3)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge2^triedge1)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge2^triedge2)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge2^triedge3)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge3^triedge1)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge3^triedge2)) return false;
	if (!isect(boxpoints, 8, t->v, 3, boxedge3^triedge3)) return false;
	
	// sino está dentro
	return true;
}*/

#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;

int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])	// -NJMP-
{
  int q;
  float vmin[3],vmax[3],v;
  for(q=X;q<=Z;q++)
  {
    v=vert[q];					// -NJMP-
    if(normal[q]>0.0f)
    {
      vmin[q]=-maxbox[q] - v;	// -NJMP-
      vmax[q]= maxbox[q] - v;	// -NJMP-
    }
    else
    {
      vmin[q]= maxbox[q] - v;	// -NJMP-
      vmax[q]=-maxbox[q] - v;	// -NJMP-
    }
  }
  if(DOT(normal,vmin)>0.0f) return false;	// -NJMP-
  if(DOT(normal,vmax)>=0.0f) return true;	// -NJMP-
  
  return 0;
}

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			       	   \
	p2 = a*v2[Y] - b*v2[Z];			       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return false;
#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0[Y] - b*v0[Z];			           \
	p1 = a*v1[Y] - b*v1[Z];			       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return false;
/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p2 = -a*v2[X] + b*v2[Z];	       	       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return false;
#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0[X] + b*v0[Z];		      	   \
	p1 = -a*v1[X] + b*v1[Z];	     	       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
	if(min>rad || max<-rad) return false;
/*======================== Z-tests ========================*/
#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1[X] - b*v1[Y];			           \
	p2 = a*v2[X] - b*v2[Y];			       	   \
    if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return false;
#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0[X] - b*v0[Y];				   \
	p1 = a*v1[X] - b*v1[Y];			           \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
	if(min>rad || max<-rad) return false;

bool BoxTriangleIntersect(Vector3* boxpoints,Triangle* t)
{
	float boxcenter[3];
	boxcenter[0]=(boxpoints[0][0]+boxpoints[7][0])*0.5f;
	boxcenter[1]=(boxpoints[0][1]+boxpoints[7][1])*0.5f;
	boxcenter[2]=(boxpoints[0][2]+boxpoints[7][2])*0.5f;

	float boxhalfsize[3];
	boxhalfsize[0]=(boxpoints[7][0]-boxpoints[0][0])*0.5f+0.001f; // para evitar fallos de precisión
	boxhalfsize[1]=(boxpoints[7][1]-boxpoints[0][1])*0.5f+0.001f;
	boxhalfsize[2]=(boxpoints[7][2]-boxpoints[0][2])*0.5f+0.001f;

	float triverts[3][3];
	triverts[0][0]=t->v[0][0];
	triverts[0][1]=t->v[0][1];
	triverts[0][2]=t->v[0][2];
	triverts[1][0]=t->v[1][0];
	triverts[1][1]=t->v[1][1];
	triverts[1][2]=t->v[1][2];
	triverts[2][0]=t->v[2][0];
	triverts[2][1]=t->v[2][1];
	triverts[2][2]=t->v[2][2];

 /*    use separating axis theorem to test overlap between triangle and box */
  /*    need to test for overlap in these directions: */
  /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
  /*       we do not even need to test these) */
  /*    2) normal of the triangle */
  /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
  /*       this gives 3x3=9 more tests */

	float v0[3],v1[3],v2[3];
//   float axis[3];
   float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
   float normal[3],e0[3],e1[3],e2[3];

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */
   SUB(v0,triverts[0],boxcenter);
   SUB(v1,triverts[1],boxcenter);
   SUB(v2,triverts[2],boxcenter);

   /* compute triangle edges */
   SUB(e0,v1,v0);      /* tri edge 0 */
   SUB(e1,v2,v1);      /* tri edge 1 */
   SUB(e2,v0,v2);      /* tri edge 2 */

   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   fex = fabsf(e0[X]);
   fey = fabsf(e0[Y]);
   fez = fabsf(e0[Z]);
   AXISTEST_X01(e0[Z], e0[Y], fez, fey);
   AXISTEST_Y02(e0[Z], e0[X], fez, fex);
   AXISTEST_Z12(e0[Y], e0[X], fey, fex);

   fex = fabsf(e1[X]);
   fey = fabsf(e1[Y]);
   fez = fabsf(e1[Z]);
   AXISTEST_X01(e1[Z], e1[Y], fez, fey);
   AXISTEST_Y02(e1[Z], e1[X], fez, fex);
   AXISTEST_Z0(e1[Y], e1[X], fey, fex);

   fex = fabsf(e2[X]);
   fey = fabsf(e2[Y]);
   fez = fabsf(e2[Z]);
   AXISTEST_X2(e2[Z], e2[Y], fez, fey);
   AXISTEST_Y1(e2[Z], e2[X], fez, fex);
   AXISTEST_Z12(e2[Y], e2[X], fey, fex);

   /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in X-direction */
   FINDMINMAX(v0[X],v1[X],v2[X],min,max);
   if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return false;

   /* test in Y-direction */
   FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
   if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return false;

   /* test in Z-direction */
   FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
   if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return false;

   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */
   CROSS(normal,e0,e1);
   // -NJMP- (line removed here)
   if(!planeBoxOverlap(normal,v0,boxhalfsize)) return false;	// -NJMP-

   return true;   /* box and triangle overlaps */
}

// MAL
/*float DistancePointTriangle(const Vector3& p, Triangle* tri)
{
	Vector3 E0=tri->v[1]-tri->v[0];
	Vector3 E1=tri->v[2]-tri->v[0];

	Vector3 D=tri->v[0]-p;
	float a=E0*E0;
	float b=E0*E1;
	float c=E1*E1;
	float d=E0*D;
	float e=E1*D;
	//float f=D*D;

	float det=a*c-b*b;
	float s=b*e-c*d;
	float t=b*d-a*e;

	float tmp0,tmp1;
	float numer,denom;

	Vector3 normal;

	if ((s+t)<=det)
	{
		if (s<0.0f)
		{
			if (t<0.0f)
			{
				// región 4
				normal=tri->n[0];
				goto region246;
			}
			else
			{
				//región 3
				normal=(tri->n[0]+tri->n[2]).Normalize();
				s=0.0f;
				t=(e>=0.0f)?0.0f:((-e>=c)?1.0f:(-e/c));
			}
		}
		else if (t<0.0f)
		{
			//región 5
			normal=(tri->n[0]+tri->n[1]).Normalize();
			t=0.0f;
			s=(d>=0.0f)?0.0f:((-d>=a)?1.0f:(-d/a));
		}
		else
		{
			// región 0
			normal=tri->fn;
			det=1.0f/det;
			s*=det;
			t*=det;
		}
	}
	else
	{
		if (s<0.0f)
		{
			// región 2
			normal=tri->n[2];
			goto region246;
		}
		else if (t<0.0f)
		{
			// región 6
			normal=tri->n[1];
			goto region246;
		}
		else
		{
			// región 1
			normal=(tri->n[1]+tri->n[2]).Normalize();
			tmp0=b+d;
			tmp1=c+e;
			numer=tmp1-tmp0;
			if (numer<=0.0f)
			{
				s=0.0f;
			}
			else
			{
				denom=a-2.0f*b+c;
				s=(numer>=denom)?1.0f:(numer/denom);
			}
			t=1.0f-s;
		}
	}
	goto end;

region246:
	tmp0=b+d;
	tmp1=c+e;
	if (tmp1>tmp0)
	{
		numer=tmp1-tmp0;
		denom=a-2*b+c;
		s=(numer>=denom)?1.0f:(numer/denom);
		t=1.0f-s;
	}
	else
	{
		s=0.0f;
		t=(tmp1<=0.0f)?1.0f:(e>=0.0f?0.0f:(-e/c));
	}
	goto end;

end:
	Vector3 puntoentriangulo=tri->v[0]+E0*s+E1*t;
	puntoentriangulo=p-puntoentriangulo;
	float dist=puntoentriangulo.Module();
	if (puntoentriangulo*normal<0)
		dist=-dist;
	return dist;
}*/

float DistancePointTriangle(const Vector3& p, Triangle* tri)
{
	Vector3 kDiff=tri->v[0]-p;
    Vector3 kEdge0=tri->v[1]-tri->v[0];
    Vector3 kEdge1=tri->v[2]-tri->v[0];
    float fA00=kEdge0*kEdge0;
    float fA01=kEdge0*kEdge1;
    float fA11=kEdge1*kEdge1;
    float fB0=kDiff*kEdge0;
    float fB1=kDiff*kEdge1;
    float fC=kDiff*kDiff;
    float fDet=fabs(fA00*fA11-fA01*fA01);
    float fS=fA01*fB1-fA11*fB0;
    float fT=fA01*fB0-fA00*fB1;
    float fSqrDistance;

	Vector3 normal;

    if (fS+fT<=fDet)
    {
        if (fS<0.0f)
        {
            if (fT<0.0f)  // region 4
            {
				normal=tri->n[0];
                if (fB0<0.0f)
                {
                    fT=0.0f;
                    if (-fB0>=fA00)
                    {
                        fS=1.0f;
                        fSqrDistance=fA00+2.0f*fB0+fC;
                    }
                    else
                    {
                        fS=-fB0/fA00;
                        fSqrDistance=fB0*fS+fC;
                    }
                }
                else
                {
                    fS=0.0f;
                    if (fB1>=0.0f)
                    {
                        fT=0.0f;
                        fSqrDistance=fC;
                    }
                    else if (-fB1>=fA11)
                    {
                        fT=1.0f;
                        fSqrDistance=fA11+2.0f*fB1+fC;
                    }
                    else
                    {
                        fT=-fB1/fA11;
                        fSqrDistance=fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
				normal=tri->na[2];
                fS=0.0f;
                if (fB1>=0.0f)
                {
                    fT=0.0f;
                    fSqrDistance=fC;
                }
                else if (-fB1>=fA11)
                {
                    fT=1.0f;
                    fSqrDistance=fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT=-fB1/fA11;
                    fSqrDistance=fB1*fT+fC;
                }
            }
        }
        else if (fT<0.0f)  // region 5
        {
			normal=tri->na[0];
            fT=0.0f;
            if (fB0>=0.0f)
            {
                fS=0.0f;
                fSqrDistance=fC;
            }
            else if (-fB0>=fA00)
            {
                fS=1.0f;
                fSqrDistance=fA00+2.0f*fB0+fC;
            }
            else
            {
                fS=-fB0/fA00;
                fSqrDistance=fB0*fS+fC;
            }
        }
        else  // region 0
        {
			normal=tri->fn;
            // minimum at interior point
            fDet=1.0f/fDet;
            fS*=fDet;
            fT*=fDet;
            fSqrDistance=fS*(fA00*fS+fA01*fT+2.0f*fB0)+fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
    }
    else
    {
        float fTmp0, fTmp1, fNumer, fDenom;

        if (fS<0.0f)  // region 2
        {
			normal=tri->n[2];
            fTmp0=fA01+fB0;
            fTmp1=fA11+fB1;
            if (fTmp1>fTmp0)
            {
                fNumer=fTmp1-fTmp0;
                fDenom=fA00-2.0f*fA01+fA11;
                if (fNumer>=fDenom)
                {
                    fS=1.0f;
                    fT=0.0f;
                    fSqrDistance=fA00+2.0f*fB0+fC;
                }
                else
                {
                    fS=fNumer/fDenom;
                    fT=1.0f-fS;
                    fSqrDistance=fS*(fA00*fS+fA01*fT+2.0f*fB0)+fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
            else
            {
                fS=0.0f;
                if (fTmp1<=0.0f)
                {
                    fT=1.0f;
                    fSqrDistance=fA11+2.0f*fB1+fC;
                }
                else if (fB1>=0.0f)
                {
                    fT=0.0f;
                    fSqrDistance=fC;
                }
                else
                {
                    fT=-fB1/fA11;
                    fSqrDistance=fB1*fT+fC;
                }
            }
        }
        else if (fT<0.0f)  // region 6
        {
			normal=tri->n[1];
            fTmp0=fA01+fB1;
            fTmp1=fA00+fB0;
            if (fTmp1>fTmp0)
            {
                fNumer=fTmp1-fTmp0;
                fDenom=fA00-2.0f*fA01+fA11;
                if (fNumer>=fDenom)
                {
                    fT=1.0f;
                    fS=0.0f;
                    fSqrDistance=fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT=fNumer/fDenom;
                    fS=1.0f-fT;
                    fSqrDistance=fS*(fA00*fS+fA01*fT+2.0f*fB0)+fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
            else
            {
                fT=0.0f;
                if (fTmp1<=0.0f)
                {
                    fS=1.0f;
                    fSqrDistance=fA00+2.0f*fB0+fC;
                }
                else if (fB0>=0.0f)
                {
                    fS=0.0f;
                    fSqrDistance=fC;
                }
                else
                {
                    fS=-fB0/fA00;
                    fSqrDistance=fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
			normal=tri->na[1];
            fNumer=fA11+fB1-fA01-fB0;
            if (fNumer<=0.0f)
            {
                fS=0.0f;
                fT=1.0f;
                fSqrDistance=fA11+2.0f*fB1+fC;
            }
            else
            {
                fDenom=fA00-2.0f*fA01+fA11;
                if (fNumer>=fDenom)
                {
                    fS=1.0f;
                    fT=0.0f;
                    fSqrDistance=fA00+2.0f*fB0+fC;
                }
                else
                {
                    fS=fNumer/fDenom;
                    fT=1.0f-fS;
                    fSqrDistance=fS*(fA00*fS+fA01*fT+2.0f*fB0)+fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
        }
    }

    // account for numerical round-off error
    if (fSqrDistance<0.0f)
    {
        fSqrDistance=0.0f;
    }

    //m_kClosestPoint0 = m_rkVector;
    //m_kClosestPoint1 = m_rkTriangle.V[0] + fS*kEdge0 + fT*kEdge1;
    //m_afTriangleBary[1] = fS;
    //m_afTriangleBary[2] = fT;
    //m_afTriangleBary[0] = (Real)1.0 - fS - fT;

	Vector3 puntoentriangulo=tri->v[0]+kEdge0*fS+kEdge1*fT;
	puntoentriangulo=p-puntoentriangulo;
	float dist=sqrtf(fSqrDistance);
	if (puntoentriangulo*normal<0)
		dist=-dist;
	return dist;
}