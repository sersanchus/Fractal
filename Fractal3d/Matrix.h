#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "Vector.h"

class Matrix
{

public:

	Vector4 col[4];			// columnas de la matriz 4x4
	
	Matrix(void);
	Vector4 operator*(const Vector4&) const;
	Matrix operator*(float) const;
	Matrix operator*(const Matrix&);
	inline Vector4 &operator[](const unsigned int column) const {return (Vector4&)(col[column]);}
	Matrix operator=(const Matrix&);
	bool operator!=(const Matrix&) const;
	void LoadIdentity(void);
	void LoadRotationXMatrix(float);
	void LoadRotationYMatrix(float);
	void LoadRotationZMatrix(float);
	void RotateX(float);
	void RotateY(float);
	void RotateZ(float);
	void Translate(const Vector3&);
	void Scale(float);
	float Determinant(void) const;
	Matrix Adjoint(void) const;
	Matrix Invert(void) const;

};

#endif