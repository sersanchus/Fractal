#include "Matrix.h"

Matrix::Matrix(void)
{
	col[0]=Vector4(1,0,0,0);
	col[1]=Vector4(0,1,0,0);
	col[2]=Vector4(0,0,1,0);
	col[3]=Vector4(0,0,0,1);
}

Vector4 Matrix::operator*(const Vector4 &v) const
{ 
	return Vector4(
	col[0].x*v.x + col[1].x*v.y + col[2].x*v.z + col[3].x*v.w,
	col[0].y*v.x + col[1].y*v.y + col[2].y*v.z + col[3].y*v.w,
	col[0].z*v.x + col[1].z*v.y + col[2].z*v.z + col[3].z*v.w,
	col[0].w*v.x + col[1].w*v.y + col[2].w*v.z + col[3].w*v.w
	);
}

Matrix Matrix::operator*(float s) const
{
	Matrix res;		
	res.col[0]=col[0]*s;
	res.col[1]=col[1]*s;
	res.col[2]=col[2]*s;
	res.col[3]=col[3]*s;
	return res;
}

Matrix Matrix::operator*(const Matrix &tm) 
{
	Matrix res;		
	for (unsigned int c=0; c<4; c++)
		for (unsigned int f=0; f<4; f++)
		{
			float ff=0.0;
			ff+=col[0][f]*tm[c][0];
			ff+=col[1][f]*tm[c][1];
			ff+=col[2][f]*tm[c][2];
			ff+=col[3][f]*tm[c][3];
			res[c][f]=ff;
		}
	return res;
}

Matrix Matrix::operator=(const Matrix &tm)
{
	col[0]=tm[0];
	col[1]=tm[1];
	col[2]=tm[2];
	col[3]=tm[3];
	return (*this);
}

bool Matrix::operator!=(const Matrix &tm) const
{
	if (col[0]!=tm[0]) return false;
	if (col[1]!=tm[1]) return false;
	if (col[2]!=tm[2]) return false;
	if (col[3]!=tm[3]) return false;
	return true;
}

void Matrix::LoadIdentity(void)
{
	col[0][0]=1.0; col[1][0]=0.0; col[2][0]=0.0; col[3][0]=0.0;
	col[0][1]=0.0; col[1][1]=1.0; col[2][1]=0.0; col[3][1]=0.0;
	col[0][2]=0.0; col[1][2]=0.0; col[2][2]=1.0; col[3][2]=0.0;
	col[0][3]=0.0; col[1][3]=0.0; col[2][3]=0.0; col[3][3]=1.0;
}

void Matrix::LoadRotationXMatrix(float angle)
{
	float cosa=cosf(angle*float(0.017453292519943295769236907684888));
	float sina=sinf(angle*float(0.017453292519943295769236907684888));
	col[0][0]=1.0; col[1][0]=0.0; col[2][0]=0.0; col[3][0]=0.0;
	col[0][1]=0.0; col[1][1]=cosa; col[2][1]=-sina;col[3][1]=0.0;
	col[0][2]=0.0; col[1][2]=sina; col[2][2]=cosa; col[3][2]=0.0;
	col[0][3]=0.0; col[1][3]=0.0; col[2][3]=0.0; col[3][3]=1.0;
}

void Matrix::LoadRotationYMatrix(float angle)
{
	float cosa=cosf(angle*float(0.017453292519943295769236907684888));
	float sina=sinf(angle*float(0.017453292519943295769236907684888));
	col[0][0]=cosa; col[1][0]=0.0; col[2][0]=sina; col[3][0]=0.0;
	col[0][1]=0.0; col[1][1]=1.0; col[2][1]=0.0; col[3][1]=0.0;
	col[0][2]=-sina;col[1][2]=0.0; col[2][2]=cosa; col[3][2]=0.0;
	col[0][3]=0.0; col[1][3]=0.0; col[2][3]=0.0; col[3][3]=1.0;
}

void Matrix::LoadRotationZMatrix(float angle)
{
	float cosa=cosf(angle*float(0.017453292519943295769236907684888));
	float sina=sinf(angle*float(0.017453292519943295769236907684888));
	col[0][0]=cosa; col[1][0]=-sina;col[2][0]=0.0; col[3][0]=0.0;
	col[0][1]=sina; col[1][1]=cosa; col[2][1]=0.0; col[3][1]=0.0;
	col[0][2]=0.0; col[1][2]=0.0; col[2][2]=1.0; col[3][2]=0.0;
	col[0][3]=0.0; col[1][3]=0.0; col[2][3]=0.0; col[3][3]=1.0;
}

void Matrix::RotateX(float angle)
{
	Matrix tr;
	tr.LoadRotationXMatrix(angle);
	operator=( (*this)*tr );
}

void Matrix::RotateY(float angle)
{
	Matrix tr;
	tr.LoadRotationYMatrix(angle);
	operator=( (*this)*tr );
}

void Matrix::RotateZ(float angle)
{
	Matrix tr;
	tr.LoadRotationZMatrix(angle);
	operator=( (*this)*tr );
}

void Matrix::Translate(const Vector3 &tv)
{
	Matrix tr;
	tr.col[3]=tv;
	operator=( (*this)*tr );
}

void Matrix::Scale(float f)
{
	Matrix tr;
	tr.col[0][0]=f;
	tr.col[1][1]=f;
	tr.col[2][2]=f;
	operator=( (*this)*tr );
}

float Determinant2x2(float a,float b,float c,float d)
{
	return a*d-b*c;
}

float Determinant3x3(float a1,float a2,float a3,float b1,float b2,float b3,float c1,float c2,float c3)
{
	return a1*Determinant2x2(b2,b3,c2,c3)-b1*Determinant2x2(a2,a3,c2,c3)+c1*Determinant2x2(a2,a3,b2,b3);
}

float Matrix::Determinant(void) const
{
    float ans = 
		col[0].x * Determinant3x3( col[1].y, col[1].z, col[1].w, col[2].y, col[2].z, col[2].w, col[3].y, col[3].z, col[3].w)
        - col[1].x * Determinant3x3( col[0].y, col[0].z, col[0].w, col[2].y, col[2].z, col[2].w, col[3].y, col[3].z, col[3].w)
        + col[2].x * Determinant3x3( col[0].y, col[0].z, col[0].w, col[1].y, col[1].z, col[1].w, col[3].y, col[3].z, col[3].w)
        - col[3].x * Determinant3x3( col[0].y, col[0].z, col[0].w, col[1].y, col[1].z, col[1].w, col[2].y, col[2].z, col[2].w);
    return ans;
}

Matrix Matrix::Adjoint(void) const
{
	Matrix ret;
    ret.col[0].x=   Determinant3x3( col[1].y, col[1].z, col[1].w, col[2].y, col[2].z, col[2].w, col[3].y, col[3].z, col[3].w);
    ret.col[0].y= - Determinant3x3( col[0].y, col[0].z, col[0].w, col[2].y, col[2].z, col[2].w, col[3].y, col[3].z, col[3].w);
    ret.col[0].z=   Determinant3x3( col[0].y, col[0].z, col[0].w, col[1].y, col[1].z, col[1].w, col[3].y, col[3].z, col[3].w);
    ret.col[0].w= - Determinant3x3( col[0].y, col[0].z, col[0].w, col[1].y, col[1].z, col[1].w, col[2].y, col[2].z, col[2].w);
    ret.col[1].x= - Determinant3x3( col[1].x, col[1].z, col[1].w, col[2].x, col[2].z, col[2].w, col[3].x, col[3].z, col[3].w);
    ret.col[1].y=   Determinant3x3( col[0].x, col[0].z, col[0].w, col[2].x, col[2].z, col[2].w, col[3].x, col[3].z, col[3].w);
    ret.col[1].z= - Determinant3x3( col[0].x, col[0].z, col[0].w, col[1].x, col[1].z, col[1].w, col[3].x, col[3].z, col[3].w);
    ret.col[1].w=   Determinant3x3( col[0].x, col[0].z, col[0].w, col[1].x, col[1].z, col[1].w, col[2].x, col[2].z, col[2].w);
    ret.col[2].x=   Determinant3x3( col[1].x, col[1].y, col[1].w, col[2].x, col[2].y, col[2].w, col[3].x, col[3].y, col[3].w);
    ret.col[2].y= - Determinant3x3( col[0].x, col[0].y, col[0].w, col[2].x, col[2].y, col[2].w, col[3].x, col[3].y, col[3].w);
    ret.col[2].z=   Determinant3x3( col[0].x, col[0].y, col[0].w, col[1].x, col[1].y, col[1].w, col[3].x, col[3].y, col[3].w);
    ret.col[2].w= - Determinant3x3( col[0].x, col[0].y, col[0].w, col[1].x, col[1].y, col[1].w, col[2].x, col[2].y, col[2].w);
    ret.col[3].x= - Determinant3x3( col[1].x, col[1].y, col[1].z, col[2].x, col[2].y, col[2].z, col[3].x, col[3].y, col[3].z);
    ret.col[3].y=   Determinant3x3( col[0].x, col[0].y, col[0].z, col[2].x, col[2].y, col[2].z, col[3].x, col[3].y, col[3].z);
    ret.col[3].z= - Determinant3x3( col[0].x, col[0].y, col[0].z, col[1].x, col[1].y, col[1].z, col[3].x, col[3].y, col[3].z);
    ret.col[3].w=   Determinant3x3( col[0].x, col[0].y, col[0].z, col[1].x, col[1].y, col[1].z, col[2].x, col[2].y, col[2].z);
	return ret;
}

Matrix Matrix::Invert(void) const
{
	float det=Determinant();
	Matrix ret=Adjoint();
	return ret*(1.0f/det);
}