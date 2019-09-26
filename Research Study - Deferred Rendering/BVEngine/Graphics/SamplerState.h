#ifndef SAMPLERSTATE_H
#define SAMPLERSTATE_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class SamplerState
	{
	public:
		SamplerState();
		~SamplerState();

		GraphicsResult Create(const D3D11_SAMPLER_DESC & samplerStateDesc);

		void Release();

		inline ID3D11SamplerState * const Get() const { return m_pSamplerState.Get(); }

	private:
		SamplerState(const SamplerState & other) = delete;
		SamplerState(SamplerState && other) = delete;
		SamplerState & operator = (const SamplerState & other) = delete;
		SamplerState & operator = (SamplerState && other) = delete;

	private:
		ComPtr<ID3D11SamplerState> m_pSamplerState;
	};
}
#endif