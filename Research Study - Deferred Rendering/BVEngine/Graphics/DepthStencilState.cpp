#include "DepthStencilState.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	DepthStencilState::DepthStencilState()
	{
	}


	DepthStencilState::~DepthStencilState()
	{
		Release();
	}

	GraphicsResult DepthStencilState::Create(const D3D11_DEPTH_STENCIL_DESC & depthStencilStateDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "DepthStencilState::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateDepthStencilState(&depthStencilStateDesc, m_pDepthStencilState.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::DEPTH_STENCIL_STATE_FAIL;
		}

		return result;
	}
	void DepthStencilState::Release()
	{
		if (m_pDepthStencilState)
		{
			m_pDepthStencilState.Reset();
			m_pDepthStencilState = nullptr;
		}
	}
}