#include "stdlib.h"
#include "Octree.h"
#include <string.h>

OctreeNode::OctreeNode(float mx,float my,float mz,float Mx,float My,float Mz)
{
	numtriangles=0;
	tri=NULL;
	n[0]=n[1]=n[2]=n[3]=n[4]=n[5]=n[6]=n[7]=NULL;
	/*minx=mx;miny=my;minz=mz;
	maxx=Mx;maxy=My;maxz=Mz;*/
	boxpoints[0][0]=mx;boxpoints[0][1]=my;boxpoints[0][2]=mz;
	boxpoints[1][0]=Mx;boxpoints[1][1]=my;boxpoints[1][2]=mz;
	boxpoints[2][0]=mx;boxpoints[2][1]=My;boxpoints[2][2]=mz;
	boxpoints[3][0]=mx;boxpoints[3][1]=my;boxpoints[3][2]=Mz;
	boxpoints[4][0]=Mx;boxpoints[4][1]=My;boxpoints[4][2]=mz;
	boxpoints[5][0]=mx;boxpoints[5][1]=My;boxpoints[5][2]=Mz;
	boxpoints[6][0]=Mx;boxpoints[6][1]=my;boxpoints[6][2]=Mz;
	boxpoints[7][0]=Mx;boxpoints[7][1]=My;boxpoints[7][2]=Mz;
	pos=Vector3((mx+Mx)*0.5f,(my+My)*0.5f,(mz+Mz)*0.5f);
	level=0;
	state=NODE_UNKNOWN;
}

OctreeNode::~OctreeNode()
{
	if (n[0]) delete n[0];
	if (n[1]) delete n[1];
	if (n[2]) delete n[2];
	if (n[3]) delete n[3];
	if (n[4]) delete n[4];
	if (n[5]) delete n[5];
	if (n[6]) delete n[6];
	if (n[7]) delete n[7];
	
	TriangleNode* aux;
	while(tri)
	{
		aux=tri;
		tri=tri->next;
		delete aux;		
	}
}

void OctreeNode::ConstructRecursive(unsigned int deep)
{
	n[0]=new OctreeNode(boxpoints[0][0],	boxpoints[0][1],	boxpoints[0][2],	pos.x,	pos.y,	pos.z);
	n[1]=new OctreeNode(pos.x,	boxpoints[0][1],	boxpoints[0][2],	boxpoints[7][0],	pos.y,	pos.z);
	n[2]=new OctreeNode(boxpoints[0][0],	pos.y,	boxpoints[0][2],	pos.x,	boxpoints[7][1],	pos.z);
	n[3]=new OctreeNode(pos.x,	pos.y,	boxpoints[0][2],	boxpoints[7][0],	boxpoints[7][1],	pos.z);
	n[4]=new OctreeNode(boxpoints[0][0],	boxpoints[0][1],	pos.z,	pos.x,	pos.y,	boxpoints[7][2]);
	n[5]=new OctreeNode(pos.x,	boxpoints[0][1],	pos.z,	boxpoints[7][0],	pos.y,	boxpoints[7][2]);
	n[6]=new OctreeNode(boxpoints[0][0],	pos.y,	pos.z,	pos.x,	boxpoints[7][1],	boxpoints[7][2]);
	n[7]=new OctreeNode(pos.x,	pos.y,	pos.z,	boxpoints[7][0],	boxpoints[7][1],	boxpoints[7][2]);
	n[0]->level=n[1]->level=n[2]->level=n[3]->level=n[4]->level=n[5]->level=n[6]->level=n[7]->level=level+1;

	if (state==NODE_UNKNOWN)
		int i=3;

	if (state==NODE_ALL_OUT || state==NODE_ALL_IN)
	{
		n[0]->state=n[1]->state=n[2]->state=n[3]->state=n[4]->state=n[5]->state=n[6]->state=n[7]->state=state;
	}
	else
	{
		TriangleNode* iteratri;
		OctreeNode* auxnode;
		TriangleNode* auxtri;
		
		//Triangle* mintri;
		float mindist;
		float distaux;
		bool col;

		//float maxprodescalar;
		
		for (unsigned int numchild=0;numchild<8;numchild++)
		{
			auxnode=n[numchild];
			
			col=false;
			//mintri=NULL;
			mindist=9999999999.9f;

			//maxprodescalar=0.0f;
			
			iteratri=tri;
			while(iteratri)
			{
				if (BoxTriangleIntersect(auxnode->boxpoints,iteratri->t))
				{
					auxtri=new TriangleNode;
					auxtri->t=iteratri->t;
					
					distaux=DistancePointTriangle(auxnode->pos,iteratri->t/*auxtri->t*/);
					/*if (fabs(distaux-mindist)<0.000001f)
					{
						float prodescalar=(((iteratri->t->v[1]-iteratri->t->v[0])^(iteratri->t->v[2]-iteratri->t->v[0])).Normalize()*(auxnode->pos-iteratri->t->v[0]).Normalize());
						if (fabs(prodescalar)>fabs(maxprodescalar))
						{
							maxprodescalar=prodescalar;
							//mindist=distaux;
							mintri=iteratri->t;
							col=true;
						}
					}
					else*/if (fabs(distaux)<fabs(mindist))
					{
						//float prodescalar=(((iteratri->t->v[1]-iteratri->t->v[0])^(iteratri->t->v[2]-iteratri->t->v[0])).Normalize()*(auxnode->pos-iteratri->t->v[0]).Normalize());
						//maxprodescalar=prodescalar;
						mindist=distaux;
						//mintri=iteratri->t/*auxtri*/;
						col=true;
					}

					auxtri->next=auxnode->tri;
					auxnode->tri=auxtri;
					auxnode->numtriangles++;
				}
				iteratri=iteratri->next;
			}

			if (!col)
			{
				if (state==NODE_PART_OUT)
					auxnode->state=NODE_ALL_OUT;
				else
					auxnode->state=NODE_ALL_IN;
			}
			else
			{
				if (mindist>=0.0f)
					auxnode->state=NODE_PART_OUT;
				else
					auxnode->state=NODE_PART_IN;

				auxnode->distmin=mindist;
			}
		}
		if (level>0)
		{
			while(tri)
			{
				iteratri=tri;
				tri=tri->next;
				delete iteratri;
			}
		}
	}

	if (deep>1)
	{
		for (unsigned int numchild=0;numchild<8;numchild++)
		{
			//if (n[numchild]->numtriangles>0)
				n[numchild]->ConstructRecursive(deep-1);
		}
	}
}

static unsigned int tamaux;
static unsigned int tamtamaux;
static unsigned char* paux;

void OctreeNode::CreateVoxelsRecursive(unsigned int px,unsigned int py,unsigned int pz,unsigned int salto)
{
	if (n[0])
	{
		unsigned int saltodiv2=salto>>1;
		n[0]->CreateVoxelsRecursive(px			,py			,pz			,saltodiv2);
		n[1]->CreateVoxelsRecursive(px+salto	,py			,pz			,saltodiv2);
		n[2]->CreateVoxelsRecursive(px			,py+salto	,pz			,saltodiv2);
		n[3]->CreateVoxelsRecursive(px+salto	,py+salto	,pz			,saltodiv2);
		n[4]->CreateVoxelsRecursive(px			,py			,pz+salto	,saltodiv2);
		n[5]->CreateVoxelsRecursive(px+salto	,py			,pz+salto	,saltodiv2);
		n[6]->CreateVoxelsRecursive(px			,py+salto	,pz+salto	,saltodiv2);
		n[7]->CreateVoxelsRecursive(px+salto	,py+salto	,pz+salto	,saltodiv2);
	}
	else
	{
		unsigned char value;
		float faux;

		if (state==NODE_UNKNOWN)
		{
			int i=3;
		}
		else if (state==NODE_ALL_OUT)
		{
			value=0;
		}
		else if (state==NODE_ALL_IN)
		{
			value=255;
		}
		else if (state==NODE_PART_OUT)
		{
			//value=0;
			faux=128.0f*(1.0f-(distmin/(1.0f/63.0f)));
			if (faux<0.0f) 
				faux=0.0f;
			else if (faux>128.0f) 
				faux=128.0f;
			value=(unsigned char)(faux+0.5f);
		}
		else
		{
			//value=255;
			faux=128.0f*(1.0f-(distmin/(1.0f/63.0f)));
			if (faux<128.0f) 
				faux=128.0f;
			else if (faux>255.0f) 
				faux=255.0f;
			value=(unsigned char)(faux+0.5f);
		}
		
		paux[px*tamtamaux+(tamaux-1-py)*tamaux+(tamaux-1-pz)]=value;
	}
}

Octree::Octree(float mx,float my,float mz,float Mx,float My,float Mz)
{
	levels=0;
	root=new OctreeNode(mx,my,mz,Mx,My,Mz);
}

Octree::~Octree()
{
	if (root)
	{
		TriangleNode* aux=root->tri;
		while(aux)
		{
			delete aux->t;		
			aux=aux->next;			
		}

		delete root;
	}
}

void Octree::Construct(unsigned int deep)
{
	if (deep>0)
	{
		levels=deep;
		root->ConstructRecursive(deep);
	}
}

void Octree::CreateVoxels(unsigned int tam,unsigned char* p)
{
	if (levels>1)
	{
		unsigned int tamdiv2=tam>>1;
		unsigned int tamdiv4=tamdiv2>>1;
		tamaux=tam;
		tamtamaux=tam*tam;
		paux=p;
		root->n[0]->CreateVoxelsRecursive(0,		0,			0,			tamdiv4);
		root->n[1]->CreateVoxelsRecursive(tamdiv2,	0,			0,			tamdiv4);
		root->n[2]->CreateVoxelsRecursive(0,		tamdiv2,	0,			tamdiv4);
		root->n[3]->CreateVoxelsRecursive(tamdiv2,	tamdiv2,	0,			tamdiv4);
		root->n[4]->CreateVoxelsRecursive(0,		0,			tamdiv2,	tamdiv4);
		root->n[5]->CreateVoxelsRecursive(tamdiv2,	0,			tamdiv2,	tamdiv4);
		root->n[6]->CreateVoxelsRecursive(0,		tamdiv2,	tamdiv2,	tamdiv4);
		root->n[7]->CreateVoxelsRecursive(tamdiv2,	tamdiv2,	tamdiv2,	tamdiv4);
	}
}
