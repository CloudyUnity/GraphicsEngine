#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_2DViewMatrix = XMMatrixIdentity();
	m_reflectionViewMatrix = XMMatrixIdentity();
	m_viewMatrix = XMMatrixIdentity();
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
	pitch = m_rotationX * (float)DEG_TO_RAD;
	yaw = m_rotationY * (float)DEG_TO_RAD;
	roll = m_rotationZ * (float)DEG_TO_RAD;

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

void CameraClass::Frame(InputClass* Input, float frameTime, float camSpeed, float camRotSpeed) 
{
	float speed = camSpeed * frameTime;
	float rotSpeed = camRotSpeed * frameTime;

	float sinX = (float)sin(m_rotationX * (float)DEG_TO_RAD);
	float cosX = (float)cos(m_rotationX * (float)DEG_TO_RAD);
	float sinY = (float)sin(m_rotationY * (float)DEG_TO_RAD);
	float cosY = (float)cos(m_rotationY * (float)DEG_TO_RAD);
	float sinZ = (float)sin(m_rotationZ * (float)DEG_TO_RAD);
	float cosZ = (float)cos(m_rotationZ * (float)DEG_TO_RAD);

	if (Input->IsKeyPressed(DIK_C))
	{
		if (Input->IsKeyPressed(DIK_A))
			m_rotationZ += rotSpeed;

		if (Input->IsKeyPressed(DIK_D))
			m_rotationZ -= rotSpeed;

		if (Input->IsKeyPressed(DIK_W))
			m_rotationX += rotSpeed;

		if (Input->IsKeyPressed(DIK_S))
			m_rotationX -= rotSpeed;

		m_rotationX = (float)fmod(m_rotationX, 360);
		m_rotationZ = (float)fmod(m_rotationZ, 360);

		return;
	}

	if (Input->IsKeyPressed(DIK_A))
	{
		m_positionX -= cosY * cosZ * speed;
		m_positionY -= sinZ * speed;
		m_positionZ += sinY * cosZ * speed;
	}

	if (Input->IsKeyPressed(DIK_D))
	{
		m_positionX += cosY * cosZ * speed;
		m_positionY += sinZ * speed;
		m_positionZ -= sinY * cosZ * speed;
	}

	if (Input->IsKeyPressed(DIK_W))
	{
		m_positionX += sinY * speed;
		m_positionZ += cosY * speed;
	}

	if (Input->IsKeyPressed(DIK_S))
	{
		m_positionX -= sinY * speed;
		m_positionZ -= cosY * speed;
	}

	if (Input->IsKeyPressed(DIK_E))
		m_positionY += speed;

	if (Input->IsKeyPressed(DIK_Q))
		m_positionY -= speed;

	if (Input->IsKeyPressed(DIK_X))
		m_rotationY += rotSpeed;

	if (Input->IsKeyPressed(DIK_Z))
		m_rotationY -= rotSpeed;

	m_rotationY = (float)fmod(m_rotationY, 360);

	if (Input->IsKeyPressed(DIK_R)) {
		m_positionX = 0;
		m_positionY = 0;
		m_positionZ = -5.0f;

		m_rotationX = 0;
		m_rotationY = 0;
		m_rotationZ = 0;
	}
}

void CameraClass::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = (-1.0f * m_rotationX) * (float)DEG_TO_RAD;  // Invert for reflection
	yaw = m_rotationY * (float)DEG_TO_RAD;
	roll = m_rotationZ * (float)DEG_TO_RAD;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetReflectionViewMatrix(XMMATRIX& reflectionViewMatrix)
{
	reflectionViewMatrix = m_reflectionViewMatrix;
}