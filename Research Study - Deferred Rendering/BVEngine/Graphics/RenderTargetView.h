#ifndef RENDERTARGETVIEW_H
#define RENDERTARGETVIEW_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class RenderTargetView
	{
	public:
		RenderTargetView();
		~RenderTargetView();

		GraphicsResult Create(ID3D11Resource * const pResource, const D3D11_RENDER_TARGET_VIEW_DESC & renderTargetViewDesc);
		GraphicsResult Create(ID3D11Resource * const pResource);

		void Release();

		inline ID3D11RenderTargetView * const Get() const { return m_pRenderTargetView.Get(); }

	private:
		RenderTargetView(const RenderTargetView & other) = delete;
		RenderTargetView(RenderTargetView && other) = delete;
		RenderTargetView & operator = (const RenderTargetView & other) = delete;
		RenderTargetView & operator = (RenderTargetView && other) = delete;

	private:
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	};
}
#endif