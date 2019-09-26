#include "RenderTargetView.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	RenderTargetView::RenderTargetView()
	{
	}


	RenderTargetView::~RenderTargetView()
	{
		Release();
	}

	GraphicsResult RenderTargetView::Create(ID3D11Resource * const pResource, const D3D11_RENDER_TARGET_VIEW_DESC & renderTargetViewDesc)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "RenderTargetView::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateRenderTargetView(pResource, &renderTargetViewDesc, m_pRenderTargetView.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::RENDER_TARGET_VIEW_FAIL;
		}

		return result;
	}
	GraphicsResult RenderTargetView::Create(ID3D11Resource * const pResource)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "RenderTargetView::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		if (FAILED(pDevice->CreateRenderTargetView(pResource, nullptr, m_pRenderTargetView.ReleaseAndGetAddressOf())))
		{
			result = GraphicsResult::RENDER_TARGET_VIEW_FAIL;
		}

		return result;
	}


	void RenderTargetView::Release()
	{
		if (m_pRenderTargetView)
		{
			m_pRenderTargetView.Reset();
		}
	}
}