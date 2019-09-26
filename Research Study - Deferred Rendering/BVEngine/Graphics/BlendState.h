#ifndef BLENDSTATE_H
#define BLENDSTATE_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	enum class BlendFactor : int
	{
		ZERO = 1,
		ONE = 2,
		SRC_COLOR = 3,
		INV_SRC_COLOR = 4,
		SRC_ALPHA = 5,
		INV_SRC_ALPHA = 6,
		DEST_ALPHA = 7,
		INV_DEST_ALPHA = 8,
		DEST_COLOR = 9,
		INV_DEST_COLOR = 10,
		SRC_ALPHA_SAT = 11,
		BLEND_FACTOR = 14,
		INV_BLEND_FACTOR = 15,
		SRC1_COLOR = 16,
		INV_SRC1_COLOR = 17,
		SRC1_ALPHA = 18,
		INV_SRC1_ALPHA = 19
	};

	enum class BlendOperation : int
	{
		ADD = 1,
		SUBTRACT = 2,
		REV_SUBTRACT = 3,
		MIN = 4,
		MAX = 5
	};

	class BlendState
	{
	public:
		BlendState();
		~BlendState();

		GraphicsResult Create(const bool enable, const BlendFactor srcBlend, const BlendFactor destBlend, const BlendOperation blendOp,
			const BlendFactor srcBlendAlpha, const BlendFactor destBlendAlpha, const BlendOperation blendOpAlpha, unsigned char mask);

		GraphicsResult Create(const D3D11_BLEND_DESC & blendStateDesc);
		
		void Release();

		inline ID3D11BlendState * Get() const { return m_pBlendState.Get(); }

	private:
		BlendState(const BlendState & other) = delete;
		BlendState(BlendState && other) = delete;
		BlendState & operator = (const BlendState & other) = delete;
		BlendState & operator = (BlendState && other) = delete;

	private:
		ComPtr<ID3D11BlendState> m_pBlendState;
	};
}
#endif