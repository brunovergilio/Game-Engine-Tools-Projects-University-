#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
}

bool ConstantBuffer::Initialize(const Graphics * const &pGraphics, const UINT size)
{
	ID3D11Device *pDevice = pGraphics->GetDevice();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = size % 16 == 0 ? size : size + (16 - (size % 16));

	if (FAILED(pDevice->CreateBuffer(&bufferDesc, nullptr, m_pConstantBuffer.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	return true;
}

void ConstantBuffer::Map(const Graphics * const &pGraphics, const void *pData, const UINT size)
{
	ID3D11DeviceContext *pContext = pGraphics->GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		CopyMemory(mappedResource.pData, pData, size);
		pContext->Unmap(m_pConstantBuffer.Get(), 0);
	}
}

ID3D11Buffer *ConstantBuffer::Get() const
{
	return m_pConstantBuffer.Get();
}