#ifndef BASICSHADER_H
#define BASICSHADER_H

#include "Shader.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Sampler.h"
#include "Material.h"
#include "Light.h"

struct BasicShaderParameters : public ShaderParameters
{
	BasicShaderParameters() : useColor(false), useTexture(false), useLight(false), useNormalMapping(false), pTexture(nullptr), pNormalMap(nullptr), pSampler(nullptr) {};
	~BasicShaderParameters() {};

	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;
	XMFLOAT4X4 invWorld;

	Material material;
	Texture *pTexture;
	Texture *pNormalMap;
	Sampler *pSampler;

	bool useColor;
	bool useTexture;
	bool useLight;
	bool useNormalMapping;
};

class BasicShader final : public Shader
{
public:
	BasicShader();
	virtual ~BasicShader();
	bool Initialize(const Graphics * const pGraphics, const char * const pShaderName) override;
	void MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters) override;

	void SetPerFrameData(PointLight pointLight, XMFLOAT3 viewPos);

private:
	// Not needed
	BasicShader(const BasicShader &) = delete;
	BasicShader & operator = (const BasicShader &) = delete;

private:
	struct CBPerObject
	{
		XMFLOAT4X4 WVPMatrix;
		XMFLOAT4X4 WorldMatrix;
		XMFLOAT4X4 InvWorld;
		Material Material;
	} m_CBPerObject;

	struct CBPerFrame
	{
		PointLight PointLight;
		XMFLOAT3 ViewPos;
	} m_CBPerFrame;

	struct CBOptions
	{
		UINT useColor;
		UINT useTexture;
		UINT useLight;
		UINT useNormalMapping;
	} m_CBOptions;

	ConstantBuffer *m_pCBPerObject;
	ConstantBuffer *m_pCBPerFrame;
	ConstantBuffer *m_pCBOptions;
};

#endif