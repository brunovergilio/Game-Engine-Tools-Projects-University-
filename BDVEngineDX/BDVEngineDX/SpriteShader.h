#ifndef SPRITESHADER_H
#define SPRITESHADER_H

#include "Shader.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Sampler.h"

struct SpriteShaderParameters : public ShaderParameters
{
	SpriteShaderParameters() {};
	~SpriteShaderParameters() {};

	XMFLOAT4X4 projMatrix;
	XMFLOAT2 textureInfo;
	Texture *pTexture;
	Sampler *pSampler;
};

class SpriteShader final : public Shader
{
public:
	SpriteShader();
	virtual ~SpriteShader();
	bool Initialize(const Graphics * const pGraphics, const char * const pShaderName) override;
	void MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters) override;

private:
	// Not needed
	SpriteShader(const SpriteShader &) = delete;
	SpriteShader & operator = (const SpriteShader &) = delete;

private:
	struct CBPerObject
	{
		XMFLOAT4X4 projMatrix;
		XMFLOAT2 textureInfo;
	} m_CBPerObject;

	ConstantBuffer *m_pCBPerObject;
};

#endif