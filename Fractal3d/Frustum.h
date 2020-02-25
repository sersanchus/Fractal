#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "Vector.h"
#include "Matrix.h"

#define CULL_ALLIN		0x3f
#define CULL_OUT		0x40

class Triangle;

// clase para manipular un frustum
class Frustum
{

public:

	int wxsize,wysize;			// tamaño de la ventana
	float wxsizeinv,wysizeinv;  // 1/tamaños
	float uvwxyz[9];/*12*/		// matriz de la vista (0-8 rotación)
	float frust_planes[6][4];  	// coeficientes de los planos del frustum
	float frust_corners[8][3];  // esquinas del frustum
	Matrix vmp;					// matrix homogenea 4x4 para opengl
    Matrix vmpinv;				// vmp invertida
	int mx0,my0;				// posición anterior del ratón en drag
	float xc,yc,scale,scaleinv;	// centro de la ventana, tamaño (para efectos de rotación)
	float q_pix;				// relación entre tamaños en frustum y tamaños de pixeles en pantalla
	float invq_pix;
	float near_plane,far_plane;	// plano frontal y plano trasero referente a frustum
	float fov;
	float auxfov;
	float aspect;
#ifdef UPDATE_HEIGHT_SPLIT
	float q_pixinv;				// pantalla->3d
#endif
	
	//bool PointVisible(Point*);
	void UpdateCull(Triangle*);								// actualiza cull bits del triangulo

	void vm2planes(void);
	void pnts2plane(int,float*,float*,float*,float*);
	void drag(float x0,float y0,float x1,float y1);	// rotación trackball desde (x0,y0) a (x1,y1)
	
	Vector3 position;
	Vector3 direction;
	Vector3 right;
	Vector3 up;

	// INTERFAZ CON ENGINE

	// constructor, destructor
	Frustum(const Vector3&,const Vector3&,const Vector3&,int,int);
	~Frustum(void);
	// tamaño de ventana
	void Reshape(int,int);										// actualiza tamaño de ventana
	// posición
	inline void SetPosition(const Vector3& v){position=v;}
	inline Vector3 GetPosition(void){return position;}
	// orientación
	void Rotate(Vector3,float);
	// orientación (lookat,up)
	inline void SetDirection(const Vector3& v){direction=v;}
	inline Vector3 GetDirection(void){return direction;}
	inline void SetUpVector(const Vector3& v){up=v;}
	inline Vector3 GetUpVector(void){return up;}
	// orientación (angulos de euler)
	void SetRollPitchYaw(float,float,float);
	float GetRoll(void);
	float GetPitch(void);
	float GetYaw(void);
	// fov
	void SetFov(float); // en radianes
	// transformaciones 3d 2d, 2d 3d
	Vector2 WorldToScreen(Vector3);
	Vector3 ScreenToWorld(Vector2);
	// colisiones sobre el terreno
	//bool ColOnMesh(Vector2,RoamMesh*,Vector3&);
	// actualiza matriz
	void Update(void);											// actualiza matriz de proyección
	
};

#endif
