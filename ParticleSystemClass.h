#ifndef _PARTICLE_SYSTEM_CLASS_H_
#define _PARTICLE_SYSTEM_CLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include "shaderclass.h"
#include "TextureSetClass.h"
#include <string>
#include "settings.h"
#include "MathUtilsClass.h"
using namespace DirectX;

class ParticleSystemClass
{
public:
	struct VertexParticleType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT4 color;
	};

	struct Particle
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float speed;
		float size;
		bool active;

		float startTime;
		float lifetime;		
	};

	struct ParticleSystemData
	{
		float posX, posY, posZ;
		float posDevX, posDevY, posDevZ;

		float speed, speedDev;
		float velocityX, velocityY, velocityZ;

		float colR, colG, colB;
		float colRDev, colGDev, colBDev;

		float size, sizeDev;
		int particlesPerSecond, maxParticles;

		float lifetime, lifetimeDev;
		int sizeOverLifetimeCurveIndex;
	};

public:
	ParticleSystemClass();
	~ParticleSystemClass();

	bool Initialize(ID3D11Device*, ParticleSystemData, ShaderClass*, TextureSetClass*, std::string);
	bool InitializeBuffers(ID3D11Device*);
	bool Render(ID3D11DeviceContext*, ShaderClass::ShaderParameters* params, ShaderClass* overwriteShader = nullptr);
	void RenderBuffers(ID3D11DeviceContext*);
	void Shutdown();

	bool Frame(ID3D11DeviceContext*, float);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

	void EmitParticles();
	void UpdateParticles(float);
	void KillParticles();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);
	void SetScale(float);	

	float GetRandomNumber();
	void SetBackCulling(bool);

public:
	float m_PosX, m_PosY, m_PosZ;
	float m_RotX, m_RotY, m_RotZ;
	float m_ScaleX, m_ScaleY, m_ScaleZ;
	std::string m_NameIdentifier;	

	ParticleSystemData m_data;

	bool m_backCullingDisabled;
	bool m_disabled;

private:
	VertexParticleType* m_vertices;

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Particle* m_particles;

	int m_currentParticleCount;
	float m_accumulatedTime;
	float m_totalTime;

	ShaderClass* m_Shader;
	TextureSetClass* m_Textures;	
};

#endif