#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>
#include "InputClass.h"
#include "Constants.h"
using namespace DirectX;

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	void Get2DViewMatrix(XMMATRIX&);
	void RenderReflection(float);
	void GetReflectionViewMatrix(XMMATRIX&);

	void Initialize2DView();

	void Frame(InputClass*, float);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix, m_2DViewMatrix, m_reflectionViewMatrix;
};

#endif