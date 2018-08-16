#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "Graphics.h"

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();
	bool Initialize(const Graphics * const &pGraphics, const UINT size);
	void Map(const Graphics * const &pGraphics, const void * pData, const UINT size);
	ID3D11Buffer *Get() const;

private:
	// Not needed
	ConstantBuffer(const ConstantBuffer & constantBuffer) = delete;
	ConstantBuffer & operator = (const ConstantBuffer & constantBuffer) = delete;

private:
	ComPtr<ID3D11Buffer> m_pConstantBuffer;
};

#endif