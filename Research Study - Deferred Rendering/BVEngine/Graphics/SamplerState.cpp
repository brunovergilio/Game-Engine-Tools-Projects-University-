#include "SamplerState.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	SamplerState::SamplerState()
	{
	}


	SamplerState::~SamplerState()
	{
		Release();
	}

	GraphicsResult BVGraphics::SamplerState::Create(const D3D11_SAMPLER_DESC & samplerStateDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "SamplerState::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateSamplerState(&samplerStateDesc, m_pSamplerState.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::SAMPLER_STATE_FAIL;
		}

		return result;
	}
	void SamplerState::Release()
	{
		if (m_pSamplerState)
		{
			m_pSamplerState.Reset();
			m_pSamplerState = nullptr;
		}
	}
}