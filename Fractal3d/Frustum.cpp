#include "Frustum.h"
#include "Triangle.h"

#define NORM(X,Y,Z,R) {R=1.0f/sqrtf(X*X+Y*Y+Z*Z);X*=R;Y*=R;Z*=R;} 
#define CROSS(BX,BY,BZ,CX,CY,CZ,AX,AY,AZ) {AX=BY*CZ-BZ*CY;AY=BZ*CX-BX*CZ;AZ=BX*CY-BY*CX;}
#define UX uvwxyz[0]  // rotación 0..8 
#define UY uvwxyz[1]
#define UZ uvwxyz[2]
#define VX uvwxyz[3]
#define VY uvwxyz[4]
#define VZ uvwxyz[5]
#define WX uvwxyz[6]
#define WY uvwxyz[7]
#define WZ uvwxyz[8]

Frustum::Frustum(const Vector3 &camerapos,const Vector3 &cameradir,const Vector3 &cameraup,int width, int height)
{
	// inicio del movimiento de ratón, al principio nulo
	mx0=-1;my0=-1; 

	position=camerapos;
	direction=cameradir;
	up=cameraup;

	SetFov(1.0f); // determines angle of view 2 = 114.5, 1.5 = 85.9, 1.5707 = 90 (radianes, grados)

	// inicia viewport
	Reshape(width,height);

	Update();
}

Frustum::~Frustum(void)
{
}

void Frustum::Reshape(int width, int height)
{
	wxsize=width; wysize=height;
	wxsizeinv=1.0f/(float)wxsize;
	wysizeinv=1.0f/(float)wysize;
    xc=(float)wxsize*0.5f;
    yc=(float)wysize*0.5f;
    scale=(float)(wxsize+wysize)*0.25f;
	scaleinv=1.0f/scale;
	if (wxsize>wysize)
		aspect=(float)wxsize/(float)wysize;
    else 
		aspect=(float)wysize/(float)wxsize;
}

void vm_inv(float *vm,float *vmi)
{
    int i,j,n,imax,j0,pivot[4],pivot1[4];
    float a[4][4],b[4][4],q,qmax;

    // set the dimension (the routine will work for any n)
    n=4;

    // copy source matrix into local copy (the copy will be modified)
    for (i=0;i<n;i++) 
	{
        for (j=0;j<n;j++) 
		{
            a[i][j]=vm[j*n+i];
            b[i][j]=(i==j?1.0f:0.0f);
        }
    }

    // indicate no pivoting has occurred yet
    for (i=0;i<n;i++) 
	{ 
		pivot[i]= -1; 
		pivot1[i]= -1; 
	}

    // do Gauss eliminations with pivoting    
    // (see any good numerical analysis book) 
    for (j0=0;j0<n;j0++) 
	{
        // find active row with largest abs(a[i][j0]) 
        for (i=0,imax= -1,qmax=0.0;i<n;i++) 
		{
            q=a[i][j0];
            if (pivot[i]<0 && (imax<0 || fabs(q)>fabs(qmax)))
            { 
				imax=i; 
				qmax=q; 
			}
        }
        pivot[imax]=j0; 
		pivot1[j0]=imax;
    
        // normalize so a[imax][j0]=1.0 
        q=1.0f/qmax; 
		for (j=0;j<n;j++) 
		{ 
			a[imax][j]*=q; 
			b[imax][j]*=q; 
		}

        // zero out a[i][j0] of other rows 
        for (i=0;i<n;i++) 
		{
            if (i!=imax) 
			{
                q=a[i][j0];
                for (j=0;j<n;j++) 
				{
                    a[i][j]-=a[imax][j]*q; 
					b[i][j]-=b[imax][j]*q;
                }
            }
        }
    }

    // unpermute inverse and store in destination matrix 
    for (i=0;i<n;i++) 
	{
        for (j=0;j<n;j++) 
		{
            vmi[j*n+i]=b[pivot1[i]][j];
        }
    }
}

void Frustum::Update(void)
{
	right=(direction^up).Normalize();

	uvwxyz[0]=right.x;
	uvwxyz[6]=up.x;
	uvwxyz[3]=direction.x;
	uvwxyz[1]=right.y;
	uvwxyz[7]=up.y;
	uvwxyz[4]=direction.y;
	uvwxyz[2]=right.z;
	uvwxyz[8]=up.z;
	uvwxyz[5]=direction.z;

	// rotate
	Matrix vmr;
	vmr.col[0]=Vector4(right.x,up.x,-direction.x,0.0f);
	vmr.col[1]=Vector4(right.y,up.y,-direction.y,0.0f);
	vmr.col[2]=Vector4(right.z,up.z,-direction.z,0.0f);
	vmr.col[3]=Vector4(0.0f,0.0f,0.0f,1.0f);

	// translate 
	Matrix vmt;
	vmt.col[0]=Vector4(1.0f,0.0f,0.0f,0.0f);
	vmt.col[1]=Vector4(0.0f,1.0f,0.0f,0.0f);
	vmt.col[2]=Vector4(0.0f,0.0f,1.0f,0.0f);
	vmt.col[3]=Vector4(-position);

    float qx,qy,qzz,qzw;//q,z_near,z_far;
	if (wxsize>wysize)
	{
		qx=1.0f;
		qy=(float)wxsize/(float)wysize;
	}
    else 
	{
		qy=1.0f;
		qx=(float)wysize/(float)wxsize;
	}
	    
	// perspective
	// q=1.5707f; // determines angle of view 2 = 114.5, 1.5 = 85.9, 1.5707 = 90 (radianes, grados)
    
	/*float near_min=2.02705e-2f;	// mínimo para el plano cercano (25 cm)
	float near_elev=1.0f;		// elevación de corte
	float near_in_elev=0.001f;	// valor de near cuando se da el corte
	if (fabs(position.y)<near_elev)
	{
		near_plane=near_min+fabs(position.y)*(near_in_elev-near_min);
	}
	else
	{
        near_plane=near_in_elev+fabs(position.y)-near_elev;
	}
	far_plane=fabs(position.y)+1.0f;*/

	near_plane=0.0001f;
	far_plane=5.0f;
   
	/*qx*=auxfov; 
	qy*=auxfov;
	qzw=1.0f/(near_plane-far_plane);
	qzz=-(far_plane+near_plane)*qzw;
	qzw*=2.0f*far_plane*near_plane;
	vmp.col[0]=Vector4(qx,0.0f,0.0f,0.0f);
	vmp.col[1]=Vector4(0.0f,qy,0.0f,0.0f);
	vmp.col[2]=Vector4(0.0f,0.0f,qzz,1.0f);
	vmp.col[3]=Vector4(0.0f,0.0f,qzw,0.0f);*/
	
	qx*=auxfov; 
	qy*=auxfov;
	qzw=1.0f/(near_plane-far_plane);
	qzz=(far_plane+near_plane)*qzw;
	qzw*=2.0f*far_plane*near_plane;
	vmp.col[0]=Vector4(qx,0.0f,0.0f,0.0f);
	vmp.col[1]=Vector4(0.0f,qy,0.0f,0.0f);
	vmp.col[2]=Vector4(0.0f,0.0f,qzz,-1.0f);
	vmp.col[3]=Vector4(0.0f,0.0f,qzw,0.0f);
	
	// multiply together 
	vmp=vmp*vmr*vmt;
	
	// compute inverse of view transform 
	//vmpinv=vmp.Invert();
	vm_inv(&vmp.col[0][0],&vmpinv.col[0][0]);
    
	// compute frustum plane-equation coefficients from view transform
    vm2planes();
}

void Frustum::Rotate(Vector3 eje,float grados) // grados en radianes
{
	float c=cosf(grados);
	float s=sinf(grados);
	float t=1-c;
	Matrix rot;
	rot.col[0]=Vector4(t*eje.x*eje.x+c,		t*eje.x*eje.y+eje.z*s,	t*eje.x*eje.z-eje.y*s,0);
	rot.col[1]=Vector4(t*eje.x*eje.y-eje.z*s,	t*eje.y*eje.y+c,		t*eje.y*eje.z+eje.x*s,0);
	rot.col[2]=Vector4(t*eje.x*eje.z+eje.y*s,	t*eje.y*eje.z-eje.x*s,	t*eje.z*eje.z+c,0);

	up=rot*up;
	direction=rot*direction;
}
void Frustum::SetRollPitchYaw(float roll,float pitch,float yaw)
{
	// roll, pitch y yaw en grados
	Matrix aux;
	aux.RotateZ(roll);		
	aux.RotateY(yaw);
	aux.RotateX(pitch);	
	direction=aux*Vector3(0.0f,0.0f,-1.0f);
	up=aux*Vector3(0.0f,1.0f,0.0f);
	right=aux*Vector3(1.0f,0.0f,0.0f);
}

float Frustum::GetRoll(void)
{
	float angulo=acosf(Vector3(direction.x,direction.y,0.0f).Normalize()%Vector3(1.0f,0.0f,0.0f));

	if (direction.y<0.0f)
		angulo=-angulo;

	return angulo;
}

float Frustum::GetPitch(void)
{
	float angulo=acosf(Vector3(0.0f,direction.y,direction.z).Normalize()%Vector3(0.0f,0.0f,-1.0f));

	if (direction.y<0.0f)
		angulo=-angulo;

	return angulo;
}

float Frustum::GetYaw(void)
{
	float angulo=acosf(Vector3(direction.x,0.0f,direction.z).Normalize()%Vector3(1.0f,0.0f,0.0f));

	if (direction.z>0.0f)
		angulo=-angulo;

	return angulo;
}

void Frustum::SetFov(float f)
{
	fov=f;
	auxfov=1.0f/tanf(fov*0.5f);
}

Vector2 Frustum::WorldToScreen(Vector3 p)
{
	/*Vector4 aux=p;
	aux=vmp*aux;
	aux.x/=aux.w; // IMPORTANTE! divido por w NO Z (que sería perspectiva?)
	aux.y/=aux.w;
	return Vector2(xc+((float)wxsize)*aux.x*0.5f,yc-((float)wysize)*aux.y*0.5f);*/

	// optimizado
	Vector4 aux=p;
	aux=vmp*aux;
	float auxf=0.5f/aux.w;
	aux.x*=auxf; // IMPORTANTE! divido por w NO Z (que sería perspectiva?)
	aux.y*=auxf;
	return Vector2(xc+((float)wxsize)*aux.x,yc-((float)wysize)*aux.y);
}

Vector3 Frustum::ScreenToWorld(Vector2 p)
{
	/*float far_plane2=far_plane*2.0f;
	Vector4 aux;
    aux.x=far_plane2*(p.x-xc)/((float)wxsize);
	aux.y=far_plane2*(yc-p.y)/((float)wysize);
	aux.z=far_plane;
	// cuanto tiene que valer w para que luego de la multiplicación sea 1
	aux.w=(1.0f-vmpinv.col[0].w*aux.x-vmpinv.col[1].w*aux.y-vmpinv.col[2].w*aux.z)/vmpinv.col[3].w;
	aux=vmpinv*aux;
	return Vector3(aux.x,aux.y,aux.z);*/
	
	Vector4 aux;
    aux.x=2.0f*(p.x-xc)/((float)wxsize);
	aux.y=2.0f*(yc-p.y)/((float)wysize);
	aux.z=1.0f;
	// cuanto tiene que valer w para que luego de la multiplicación sea 1
	aux.w=(1.0f-vmpinv.col[0].w*aux.x-vmpinv.col[1].w*aux.y-vmpinv.col[2].w*aux.z)/vmpinv.col[3].w;
	//aux.w=1.0f;
	aux=vmpinv*aux;
	return Vector3(aux.x,aux.y,aux.z);
}

/*bool Frustum::ColOnMesh(Vector2 p,RoamMesh* rm,Vector3& point)
{
	return rm->SolveCollision(position,ScreenToWorld(p),point);
}*/

/*bool Frustum::PointVisible(Point* p)
{
	unsigned char j;

	for (j=0;j<6;j++)
	{
		r=frust_planes[j][0]*p->v.x+frust_planes[j][1]*p->v.y+frust_planes[j][2]*p->v.z+frust_planes[j][3];
        if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
			return false;
   	}
	return true;
}*/

void Frustum::UpdateCull(Triangle* t)
{
	/*unsigned char j,m;
    float r;

    for (j=0,m=1;j<6;j++,m<<=1)
	{
		r=frust_planes[j][0]*t->p1->v.x+frust_planes[j][1]*t->p1->v.y+frust_planes[j][2]*t->p1->v.z+frust_planes[j][3];
        if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
		{
			t->cull=CULL_OUT;
			return;
		}
   	} 

	for (j=0,m=1;j<6;j++,m<<=1)
	{
		r=frust_planes[j][0]*t->p2->v.x+frust_planes[j][1]*t->p2->v.y+frust_planes[j][2]*t->p2->v.z+frust_planes[j][3];
        if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
		{
			t->cull=CULL_OUT;
			return;
		}
   	}

	for (j=0,m=1;j<6;j++,m<<=1)
	{
		r=frust_planes[j][0]*t->p3->v.x+frust_planes[j][1]*t->p3->v.y+frust_planes[j][2]*t->p3->v.z+frust_planes[j][3];
        if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
		{
			t->cull=CULL_OUT;
			return;
		}
   	}

	t->cull=CULL_ALLIN;*/
	
	unsigned char j,m;
	float r;

	if (t->father)
	{
		t->cull=t->father->cull;
	
		if (t->cull!=CULL_ALLIN && t->cull!=CULL_OUT) 
		{
			for (j=0,m=1;j<6;j++,m<<=1)
			{
				if (!(t->cull&m)) 
				{
					r=frust_planes[j][0]*t->center.x+frust_planes[j][1]*t->center.y+frust_planes[j][2]*t->center.z+frust_planes[j][3];
					// si la distancia al plano es mayor que el radio de la esfera envolvente...
					if ((r*r)>t->rbound) 
					{
						// distancia positiva (hacia fuera del frustum) -> todo el diamante fuera
						if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
						{
							t->cull=CULL_OUT;
							break; // made in SANCHO *TODO*
						}
						else
						{
							t->cull|=m; // IN , si la distancia es negativa se encunetra dentro del plano
						}
					}
				}
   			} 
		}
	}
	else
	{
		for (j=0,m=1;j<6;j++,m<<=1)
		{
			r=frust_planes[j][0]*t->center.x+frust_planes[j][1]*t->center.y+frust_planes[j][2]*t->center.z+frust_planes[j][3];
			// si la distancia al plano es mayor que el radio de la esfera envolvente...
			if ((r*r)>t->rbound) 
			{
				// distancia positiva (hacia fuera del frustum) -> todo el diamante fuera
				if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
				{
					t->cull=CULL_OUT;
					break; // made in SANCHO *TODO*
				}
				else
				{
					t->cull|=m; // IN , si la distancia es negativa se encunetra dentro del plano
				}
			}
		} 
	}
			

	/*unsigned char cull;

	if (t->father)
		cull=t->father->cull;
	else
		cull=CULL_ALLIN;

	if (cull!=CULL_ALLIN && cull!=CULL_OUT) 
	{
        unsigned char j,m;
	    float r;

        for (j=0,m=1;j<6;j++,m<<=1)
		{
	        if (!(cull&m)) 
			{
				r=frust_planes[j][0]*t->center.x+frust_planes[j][1]*t->center.y+frust_planes[j][2]*t->center.z+frust_planes[j][3];
				// si la distancia al plano es mayor que el radio de la esfera envolvente...
				if ((r*r)>t->rbound) 
				{
					// distancia positiva (hacia fuera del frustum) -> todo el diamante fuera
	                if (r>0.0f) // por fallos de precisión y en lugar de >0.0f *TODO*
					{
						cull=CULL_OUT;
						break; // made in SANCHO *TODO*
					}
					else
					{
						cull|=m; // IN , si la distancia es negativa se encunetra dentro del plano
					}
                }
            }
       	} 
   	}

	t->cull=cull;*/
}

void vm_apply(float *vm,float *p,float *pr)
{
    float s,a[4];

	a[0]=p[0];
	a[1]=p[1];
	a[2]=p[2];

	s=1.0f/(vm[3]*a[0]+vm[7]*a[1]+vm[11]*a[2]+vm[15]);
    pr[0]=(vm[0]*a[0]+vm[4]*a[1]+vm[8]*a[2]+vm[12])*s;
	pr[1]=(vm[1]*a[0]+vm[5]*a[1]+vm[9]*a[2]+vm[13])*s;
	pr[2]=(vm[2]*a[0]+vm[6]*a[1]+vm[10]*a[2]+vm[14])*s;
}

void Frustum::vm2planes(void)
{
    unsigned int /*j,*/k;
    //Matrix vmi;
    float g[3];
    
	// send eight frustum corners through inverse transform
    for (k=0;k<8;k++) 
    {
		frust_corners[k][0]=((k&1)?1.0f:-1.0f);
		frust_corners[k][1]=((k&2)?1.0f:-1.0f);
		frust_corners[k][2]=((k&4)?1.0f:-1.0f);
        vm_apply(&vmpinv.col[0][0],frust_corners[k],frust_corners[k]);
    }

    // get misc point in the interior of the frustum volume
    g[0]=0.5f*(frust_corners[0][0]+frust_corners[7][0]);
	g[1]=0.5f*(frust_corners[0][1]+frust_corners[7][1]);
	g[2]=0.5f*(frust_corners[0][2]+frust_corners[7][2]);

    // compute plane equation coefficients for all six faces
    pnts2plane(0,frust_corners[0],frust_corners[1],frust_corners[2],g);
    pnts2plane(1,frust_corners[0],frust_corners[2],frust_corners[4],g);
    pnts2plane(2,frust_corners[0],frust_corners[4],frust_corners[1],g);
    pnts2plane(3,frust_corners[7],frust_corners[6],frust_corners[5],g);
    pnts2plane(4,frust_corners[7],frust_corners[5],frust_corners[3],g);
    pnts2plane(5,frust_corners[7],frust_corners[3],frust_corners[6],g);

	// pixel projection factor
    float x,y,z,r,d,p2,x0,y0,z0,x1,y1,z1,xc,yc,zc;
    x0=frust_corners[4][0];
    y0=frust_corners[4][1];
    z0=frust_corners[4][2];
    x1=frust_corners[7][0];
    y1=frust_corners[7][1];
    z1=frust_corners[7][2];
    xc=0.5f*(x0+x1); yc=0.5f*(y0+y1); zc=0.5f*(z0+z1);
    // distance of center to eye, length of back diagonal, length in pixels
    x=xc-position.x; y=yc-position.y; z=zc-position.z;
    d=x*x+y*y+z*z;
    x=x1-x0; y=y1-y0; z=z1-z0;
    r=x*x+y*y+z*z;
    x=(float)wxsize; y=(float)wysize;
    p2=x*x+y*y;
    // finally determine q_pix such that
    //   pix_length=q_pix*length/dist_to_eye
    q_pix=p2*d/r;
	invq_pix=1.0f/q_pix;

#ifdef UPDATE_HEIGHT_SPLIT
	//q_pixinv=10.0f*(d/p2); // 10 pixels por frame (d/p2 es relación entre pixel y tamaño real)
	q_pixinv=0.00001f*d; // porción de la diagonal de la pantalla
#endif

}

void Frustum::pnts2plane(int i, float *pc,float *pa,float *pb,float *g)
{
    unsigned int j;
    float a[3],b[3],c[3],nx,ny,nz,nr;

	for (j=0;j<3;j++) 
	{ 
		a[j]=pa[j]-pc[j]; 
		b[j]=pb[j]-pc[j]; 
		c[j]=pc[j]; 
	}
    CROSS(a[0],a[1],a[2],b[0],b[1],b[2],nx,ny,nz)
    NORM(nx,ny,nz,nr)
    if (nx*(g[0]-c[0])+ny*(g[1]-c[1])+nz*(g[2]-c[2])>0.0f)
    { 
		nx=-nx; 
		ny=-ny; 
		nz=-nz; 
	}
    frust_planes[i][0]=nx; 
	frust_planes[i][1]=ny; 
	frust_planes[i][2]=nz;
    frust_planes[i][3]=-(nx*c[0]+ny*c[1]+nz*c[2]);
}

void Frustum::drag(float x0,float y0,float x1,float y1)
{
    float r,a,ct,st;
    float au,av,aw,ar;
    float bu,bv,bw,br;
    float pu,pv,pw,pr;
    float qu,qv,qw,qr;
    float xu,xv,xw;
    float yu,yv,yw;
    float zu,zv,zw;
    float ax,ay,az;
    float px,py,pz;
    float qx,qy,qz;
    float ax1,ay1,az1;
    float qx1,qy1,qz1;

    // get unit old and new directions, normal and tangent frame
    r=scale;
    au=(x0-xc)*scaleinv; 
	av=0.99f; // si pongo 1.0f cuando esta orientado en -y se queda enganchado *TODO* SANCHO
	aw=(yc-y0)*scaleinv;
    NORM(au,av,aw,ar);
    bu=(x1-xc)*scaleinv; 
	bv=0.99f; // si pongo 1.0f cuando esta orientado en -y se queda enganchado *TODO* SANCHO
	bw=(yc-y1)*scaleinv;
    NORM(bu,bv,bw,br);
    CROSS(bu,bv,bw,au,av,aw,pu,pv,pw);
    NORM(pu,pv,pw,pr);
    CROSS(au,av,aw,pu,pv,pw,qu,qv,qw);
    NORM(qu,qv,qw,qr);
    a=acosf(au*bu+av*bv+aw*bw);

	// get world frame in normal/tangent frame
    xu=UX; xv=VX; xw=WX;
    yu=UY; yv=VY; yw=WY;
    zu=UZ; zv=VZ; zw=WZ;
    ax=au*xu+av*xv+aw*xw; px=pu*xu+pv*xv+pw*xw; qx=qu*xu+qv*xv+qw*xw;
    ay=au*yu+av*yv+aw*yw; py=pu*yu+pv*yv+pw*yw; qy=qu*yu+qv*yv+qw*yw;
    az=au*zu+av*zv+aw*zw; pz=pu*zu+pv*zv+pw*zw; qz=qu*zu+qv*zv+qw*zw;

    // rotate world frame from a to b in aq plane
    ct=cosf(a); st=sinf(a);
    ax1=ct*ax-st*qx; qx1=st*ax+ct*qx;
    ay1=ct*ay-st*qy; qy1=st*ay+ct*qy;
    az1=ct*az-st*qz; qz1=st*az+ct*qz;
    ax=ax1; qx=qx1;
    ay=ay1; qy=qy1;
    az=az1; qz=qz1;

	// get world frame back in camera frame
    xu=ax*au+px*pu+qx*qu; xv=ax*av+px*pv+qx*qv; xw=ax*aw+px*pw+qx*qw;
    yu=ay*au+py*pu+qy*qu; yv=ay*av+py*pv+qy*qv; yw=ay*aw+py*pw+qy*qw;
    zu=az*au+pz*pu+qz*qu; zv=az*av+pz*pv+qz*qv; zw=az*aw+pz*pw+qz*qw;

    UX=xu; VX=xv; WX=xw;
    UY=yu; VY=yv; WY=yw;
    UZ=zu; VZ=zv; WZ=zw;

	direction.x=VX;
	direction.y=VY;
	direction.z=VZ;

	up.x=WX;
	up.y=WY;
	up.z=WZ;

}
