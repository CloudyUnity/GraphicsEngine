#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Initialize2DView() 
{
	Render();
	m_2DViewMatrix = m_viewMatrix;
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::Get2DViewMatrix(XMMATRIX & viewMatrix)
{
	viewMatrix = m_2DViewMatrix;
}

void CameraClass::Frame(InputClass* Input, float frameTime) 
{
	float speed = 4.0 * frameTime;
	float rotSpeed = 60.0 * frameTime;

	float forwardX = sin(m_rotationY * 0.0174532925f);
	float forwardZ = cos(m_rotationY * 0.0174532925f);

	if (Input->IsKeyPressed(DIK_A)) {
		m_positionX -= forwardZ * speed;
		m_positionZ += forwardX * speed;
	}

	if (Input->IsKeyPressed(DIK_D)) {
		m_positionX += forwardZ * speed;
		m_positionZ -= forwardX * speed;
	}

	if (Input->IsKeyPressed(DIK_W)) {
		m_positionX += forwardX * speed;
		m_positionZ += forwardZ * speed;
	}

	if (Input->IsKeyPressed(DIK_S)) {
		m_positionX -= forwardX * speed;
		m_positionZ -= forwardZ * speed;
	}

	if (Input->IsKeyPressed(DIK_E))
		m_positionY += speed;

	if (Input->IsKeyPressed(DIK_Q))
		m_positionY -= speed;

	if (Input->IsKeyPressed(DIK_X))
		m_rotationY += rotSpeed;

	if (Input->IsKeyPressed(DIK_Z))
		m_rotationY -= rotSpeed;

	if (Input->IsKeyPressed(DIK_R)) {
		m_positionX = 0;
		m_positionY = 0;
		m_positionZ = -5.0f;
		m_rotationX = 0;
		m_rotationY = 0;
		m_rotationZ = 0;
	}
}
