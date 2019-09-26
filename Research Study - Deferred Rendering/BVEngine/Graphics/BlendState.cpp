#include "BlendState.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	BlendState::BlendState()
	{
	}


	BlendState::~BlendState()
	{
		Release();
	}


	GraphicsResult BlendState::Create(const bool enable, const BlendFactor srcBlend, const BlendFactor destBlend, const BlendOperation blendOp,
		const BlendFactor srcBlendAlpha, const BlendFactor destBlendAlpha, const BlendOperation blendOpAlpha, unsigned char mask)
	{
		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0].BlendEnable = enable;
		blendDesc.RenderTarget[0].SrcBlend = (D3D11_BLEND)srcBlend;
		blendDesc.RenderTarget[0].DestBlend = (D3D11_BLEND)destBlend;
		blendDesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)blendOp;
		blendDesc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)srcBlendAlpha;
		blendDesc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)destBlendAlpha;
		blendDesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)blendOpAlpha;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = mask;

		return Create(blendDesc);
	}


	GraphicsResult BlendState::Create(const D3D11_BLEND_DESC & blendStateDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "BlendState::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateBlendState(&blendStateDesc, m_pBlendState.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::BLEND_STATE_FAIL;
		}

		return result;
	}
	void BlendState::Release()
	{
		if (m_pBlendState)
		{
			m_pBlendState.Reset();
			m_pBlendState = nullptr;
		}
	}
}