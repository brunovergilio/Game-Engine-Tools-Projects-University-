#ifndef DEPTHSTENCILSTATE_H
#define DEPTHSTENCILSTATE_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class DepthStencilState
	{
	public:
		DepthStencilState();
		~DepthStencilState();

		GraphicsResult Create(const D3D11_DEPTH_STENCIL_DESC & depthStencilStateDesc);

		void Release();

		inline ID3D11DepthStencilState * Get() const { return m_pDepthStencilState.Get(); }

	private:
		DepthStencilState(const DepthStencilState & other) = delete;
		DepthStencilState(DepthStencilState && other) = delete;
		DepthStencilState & operator = (const DepthStencilState & other) = delete;
		DepthStencilState & operator = (DepthStencilState && other) = delete;

	private:
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	};
}
#endif