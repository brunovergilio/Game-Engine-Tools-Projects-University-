#include "RasterizerState.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	RasterizerState::RasterizerState()
	{
	}


	RasterizerState::~RasterizerState()
	{
		Release();
	}


	GraphicsResult RasterizerState::Create(const CullMode cullMode, const bool frontCCW, const bool depthEnable, bool wireframe)
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.CullMode = (D3D11_CULL_MODE)cullMode;
		rasterizerDesc.FrontCounterClockwise = frontCCW;
		rasterizerDesc.DepthClipEnable = depthEnable;
		rasterizerDesc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
		rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;

		return Create(rasterizerDesc);
	}


	GraphicsResult RasterizerState::Create(const D3D11_RASTERIZER_DESC & rasterizerStateDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "RasterizerState::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateRasterizerState(&rasterizerStateDesc, m_pRasterizerState.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::RASTERIZER_STATE_FAIL;
		}

		return result;
	}
	void RasterizerState::Release()
	{
		if (m_pRasterizerState)
		{
			m_pRasterizerState.Reset();
			m_pRasterizerState = nullptr;
		}
	}
}