#ifndef __OCTREE_H__
#define __OCTREE_H__

#include "Vector.h"
#include "UtilMath.h"

#define NODE_UNKNOWN	0x00
#define NODE_ALL_OUT	0x01
#define NODE_ALL_IN		0x02
#define NODE_PART_OUT	0x03
#define NODE_PART_IN	0x04

class OctreeNode
{

public:

	Vector3 boxpoints[8];
	void ConstructRecursive(unsigned int);
	void CreateVoxelsRecursive(unsigned int,unsigned int,unsigned int,unsigned int);

	unsigned char state;

	float distmin;

	unsigned int level;
	Vector3 pos;

	//float maxx,maxy,maxz,minx,miny,minz;
	OctreeNode *n[8];

	unsigned int numtriangles;
	TriangleNode* tri;

	OctreeNode(float,float,float,float,float,float);
	~OctreeNode();

};

class Octree
{

private:

	unsigned int levels;

public:

	OctreeNode* root;

	Octree(float,float,float,float,float,float);
	~Octree();

	void Construct(unsigned int);
	void CreateVoxels(unsigned int,unsigned char*);

};

#endif