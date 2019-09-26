#ifndef BASICDEFERREDRENDERING_H
#define BASICDEFERREDRENDERING_H

#include "Engine.h"
#include "Camera.h"
#include "SpriteBatch.h"
#include "Light.h"
#include "Material.h"

using namespace BVEngine;
using namespace BVGraphics;

class BasicDeferredRendering : public Engine
{
public:
	BasicDeferredRendering(const char * const pWindowName, const unsigned int width, const unsigned int height);
	~BasicDeferredRendering();

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
	ShaderResourceView *m_pBrownRocksTexture;
	ShaderResourceView *m_pDryRocksTexture;
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

	unsigned int m_GBufferIndex;
};

#endif