#ifndef DEPTHSTENCILVIEW_H
#define DEPTHSTENCILVIEW_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class DepthStencilView
	{
	public:
		DepthStencilView();
		~DepthStencilView();

		GraphicsResult Create(ID3D11Resource * const pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC & depthStencilViewDesc);

		void Release();

		inline ID3D11DepthStencilView * const Get() const { return m_pDepthStencilView.Get(); }

	private:
		DepthStencilView(const DepthStencilView & other) = delete;
		DepthStencilView(DepthStencilView && other) = delete;
		DepthStencilView & operator = (const DepthStencilView & other) = delete;
		DepthStencilView & operator = (DepthStencilView && other) = delete;

	private:
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	};
}
#endif