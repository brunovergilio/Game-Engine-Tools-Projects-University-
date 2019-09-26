#ifndef RASTERIZERSTATE_H
#define RASTERIZERSTATE_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	enum class CullMode : int
	{
		NONE = 1,
		FRONT = 2,
		BACK = 3
	};

	class RasterizerState
	{
	public:
		RasterizerState();
		~RasterizerState();

		GraphicsResult Create(const CullMode cullMode = CullMode::BACK, const bool frontCCW = false, const bool depthEnable = true, const bool wireframe = false);
		GraphicsResult Create(const D3D11_RASTERIZER_DESC & rasterizerStateDesc);

		void Release();

		inline ID3D11RasterizerState * Get() const { return m_pRasterizerState.Get(); }

	private:
		RasterizerState(const RasterizerState & other) = delete;
		RasterizerState(RasterizerState && other) = delete;
		RasterizerState & operator = (const RasterizerState & other) = delete;
		RasterizerState & operator = (RasterizerState && other) = delete;

	private:
		ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	};
}
#endif