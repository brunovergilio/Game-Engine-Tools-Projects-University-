#ifndef ANIMBLENDSHADER_H
#define ANIMBLENDSHADER_H

#include "Shader.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Sampler.h"
#include "Material.h"
#include "Light.h"
#include "BDVModel.h"

#define MAX_NUM_JOINTS 100

struct ShaderInterpolationData
{
	ShaderInterpolationData() { ZeroMemory(this, sizeof(*this)); };
	~ShaderInterpolationData() {};

	int numJoints;

	// cbInterpolationInfo
	float lerp;
	int interpolate;
	int isBlending;
	float blendTime;

	// cbBeginAnim
	ID3D11Buffer *pBeginAnimBuffer;

	// cbEndAnim
	ID3D11Buffer *pEndAnimBuffer;

	// cbBlendAnim
	ID3D11Buffer *pBlendAnimBuffer;

	// cbJoint
	ID3D11Buffer *pJointsBuffer;
};

struct AnimBlendShaderParameters : public ShaderParameters
{
	AnimBlendShaderParameters() : transformOnHardware(false), useColor(false), useTexture(false), useLight(false), pTexture(nullptr), pSampler(nullptr) {};
	~AnimBlendShaderParameters() {};

	int numJoints;
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;
	XMFLOAT4X4 invWorld;
	Material material;
	Texture *pTexture;
	Sampler *pSampler;
	XMFLOAT4X4 *pFinalMatrices;
	ShaderInterpolationData *pInterpolationData;
	bool transformOnHardware;

	bool useColor;
	bool useTexture;
	bool useLight;
};

class AnimBlendShader final : public Shader
{
public:
	AnimBlendShader();
	virtual ~AnimBlendShader();
	bool Initialize(const Graphics * const pGraphics, const char * const pShaderName) override;
	void MapData(const Graphics * const pGraphics, const ShaderParameters * const pShaderParameters) override;
	void SetPerFrameData(PointLight pointLight, XMFLOAT3 viewPos);

protected:
	void Interpolate(const Graphics * const pGraphics, const ShaderInterpolationData * const pShaderInterpolationData);

private:
	// Not needed
	AnimBlendShader(const AnimBlendShader &) = delete;
	AnimBlendShader & operator = (const AnimBlendShader &) = delete;

private:
	// Shaders for Animation Interpolation
	ComPtr<ID3D11VertexShader> m_pVSTransform;
	ComPtr<ID3D11GeometryShader> m_pGSTransform;
	ComPtr<ID3D11InputLayout> m_pILTransform;

	// Buffers for Animation Interpolation
	ConstantBuffer *m_pCBInterpolationInfo;

	ComPtr<ID3D11Buffer> m_pJointBuffer;
	ComPtr<ID3D11Buffer> m_pStreamOutBuffer;

	// Buffers for Animation Rendering
	struct CBPerObject
	{
		XMFLOAT4X4 WVPMatrix;
		XMFLOAT4X4 WorldMatrix;
		XMFLOAT4X4 InvWorld;
		Material Material;
		XMFLOAT4 Color;
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
		UINT pad;
	} m_CBOptions;

	ConstantBuffer *m_pCBPerObject;
	ConstantBuffer *m_pCBPerFrame;
	ConstantBuffer *m_pCBSkin;
	ConstantBuffer *m_pCBOptions;
};

#endif