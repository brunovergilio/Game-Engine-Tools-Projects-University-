#ifndef INSTANCEDPOINTLIGHTS_H
#define INSTANCEDPOINTLIGHTS_H

#include "Engine.h"
#include "Camera.h"
#include "SpriteBatch.h"
#include "Light.h"
#include "Material.h"

using namespace BVEngine;
using namespace BVGraphics;

class InstancedPointLights : public Engine
{
public:
	InstancedPointLights(const char * const pWindowName, const unsigned int width, const unsigned int height);
	~InstancedPointLights();

private:
	bool StartUp() override;

	void Update() override;
	void Render() override;
	
	void CreateInstancedData();
	void CreateLights();

private:
	VertexBuffer *m_pVBSphere, *m_pVBGrid;
	IndexBuffer *m_pIBSphere, *m_pIBGrid;

	VertexBuffer *m_pVBInstancedData;

	VertexShader *m_pVS;
	VertexShader *m_pVSInstanced;
	PixelShader *m_pPS;
	InputLayout *m_pInputLayout;
	InputLayout *m_pInputLayoutInstanced;

	Camera m_Camera;

	ShaderResourceView *m_pBrownRocksTexture;
	ShaderResourceView *m_pBlueRocksTexture;
	SamplerState *m_pSamplerState;

	struct CBObject
	{
		BVFloat44 m_WVP;
		BVFloat44 m_World;
		BVFloat44 m_InvWorld;
	} m_CBObject;
	ConstantBuffer *m_pCBObject;

	struct CBObjectInstanced
	{
		BVFloat44 m_ViewProj;
	} m_CBObjectInstanced;
	ConstantBuffer *m_pCBObjectInstanced;

	Material m_Material;
	ConstantBuffer *m_pCBMaterial;

	Light m_Hems;

	Light m_PointLights[200];
	float m_Steps[200];

	unsigned int m_GBufferIndex;
};

#endif