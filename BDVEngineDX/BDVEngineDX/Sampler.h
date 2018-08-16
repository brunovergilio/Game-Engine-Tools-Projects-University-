#ifndef SAMPLER_H
#define SAMPLER_H

#include "Graphics.h"

enum FilterType
{
	Point,
	Bilinear,
	Trilinear,
	Anisotropic
};

enum AddressMode
{
	Wrap,
	Mirror,
	Clamp,
	Border
};

enum ComparisonFunction
{
	Never,
	Less,
	LessEqual,
	Equal,
	NotEqual,
	GreaterEqual,
	Greater,
	Always
};

class Sampler
{
public:
	Sampler();
	~Sampler();
	bool Initialize(const Graphics * const &pGraphics, const FilterType filterType,
		const AddressMode addressMode, const ComparisonFunction comparisonFunction, const UINT maxAnisotropy = 1);

	void Shutdown();

	ID3D11SamplerState *GetSampler() const;

private:
	// Not needed
	Sampler(const Sampler & sampler) = delete;
	Sampler & operator = (const Sampler & sampler) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

#endif // !SAMPLER_H