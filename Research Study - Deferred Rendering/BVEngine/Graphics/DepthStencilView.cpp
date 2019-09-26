#include "DepthStencilView.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	DepthStencilView::DepthStencilView()
	{
	}


	DepthStencilView::~DepthStencilView()
	{
		Release();
	}

	GraphicsResult DepthStencilView::Create(ID3D11Resource * const pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC & depthStencilViewDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "DepthStencilView::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateDepthStencilView(pResource, &depthStencilViewDesc, m_pDepthStencilView.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::DEPTH_STENCIL_VIEW_FAIL;
		}

		return result;
	}
	void DepthStencilView::Release()
	{
		if (m_pDepthStencilView)
		{
			m_pDepthStencilView.Reset();
			m_pDepthStencilView = nullptr;
		}
	}
}