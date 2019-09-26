#ifndef DSSPOTLIGHTTEST_H
#define DSSPOTLIGHTTEST_H

#include "Engine.h"
#include "Camera.h"
#include "SpriteBatch.h"
#include "Light.h"
#include "Material.h"

using namespace BVEngine;
using namespace BVGraphics;

class DSSpotLightTest : public Engine
{
public:
	DSSpotLightTest(const char * const pWindowName, const unsigned int width, const unsigned int height);
	~DSSpotLightTest();

	void Update();
	void Render();

private:
	bool StartUp() override;

private:
	VertexBuffer *m_VBSphere, *m_VBGrid, *m_VBBox, *m_VBCone;
	IndexBuffer *m_IBSphere, *m_IBGrid, *m_IBBox, *m_IBCone;

	VertexShader *m_VShader;
	PixelShader *m_PShader;
	InputLayout *m_pInputLayout;

	Camera m_Camera;

	ShaderResourceView *m_pBluewallTexture;
	SamplerState *m_pSamplerState;

	struct CBObject
	{
		BVFloat44 m_WVP;
		BVFloat44 m_World;
		BVFloat44 m_InvWorld;
	} m_CBObject;
	ConstantBuffer *m_pCBObject;

	Material m_Material;
	ConstantBuffer *m_pCBMaterial;

	Light m_Hems;
	Light m_TestSpot;
	float m_SpotDist;

	unsigned int m_GBufferIndex;
};

#endif