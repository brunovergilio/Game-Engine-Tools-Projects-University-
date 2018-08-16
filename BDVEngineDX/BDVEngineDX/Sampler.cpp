#include "Sampler.h"

Sampler::Sampler()
{
}

Sampler::~Sampler()
{
}

bool Sampler::Initialize(const Graphics * const &pGraphics, const FilterType filterType,
	const AddressMode addressMode, const ComparisonFunction comparisonFunction, const UINT maxAnisotropy)
{
	D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	switch (filterType)
	{
	case Point:
		filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case Bilinear:
		filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case Trilinear:
		filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case Anisotropic:
		filter = D3D11_FILTER_ANISOTROPIC;
		break;
	}

	D3D11_TEXTURE_ADDRESS_MODE mode = D3D11_TEXTURE_ADDRESS_WRAP;
	switch (addressMode)
	{
	case Wrap:
		mode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case Mirror:
		mode = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case Clamp:
		mode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case Border:
		mode = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	}

	D3D11_COMPARISON_FUNC function = D3D11_COMPARISON_NEVER;
	switch (comparisonFunction)
	{
	case Never:
		function = D3D11_COMPARISON_NEVER;
		break;
	case Less:
		function = D3D11_COMPARISON_LESS;
		break;
	case LessEqual:
		function = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case Equal:
		function = D3D11_COMPARISON_EQUAL;
		break;
	case NotEqual:
		function = D3D11_COMPARISON_NOT_EQUAL;
		break;
	case GreaterEqual:
		function = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case Greater:
		function = D3D11_COMPARISON_GREATER;
		break;
	case Always:
		function = D3D11_COMPARISON_ALWAYS;
		break;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = mode;
	samplerDesc.AddressV = mode;
	samplerDesc.AddressW = mode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = maxAnisotropy;
	samplerDesc.ComparisonFunc = function;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11Device *pDevice = pGraphics->GetDevice();

	if (FAILED(pDevice->CreateSamplerState(&samplerDesc, m_pSampler.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	return true;
}

void Sampler::Shutdown()
{
	m_pSampler.Reset();
}

ID3D11SamplerState *Sampler::GetSampler() const
{
	return m_pSampler.Get();
}