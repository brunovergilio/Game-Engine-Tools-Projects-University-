#include "GpuBuffer.h"
#include "RendererD3D11.h"

namespace BVGraphics
{
	GpuBuffer::GpuBuffer()
	{
	}


	GpuBuffer::~GpuBuffer()
	{
		Release();
	}


	GraphicsResult GpuBuffer::Create(void * const pData, const unsigned int bufferSize,
		const D3D11_USAGE usage, const unsigned int bindFlags, const unsigned int accessFlags,
		const unsigned int miscFlags, const unsigned int structureByteStride)
	{
		ID3D11Device *pDevice = RendererD3D11::GetMainInstance()->GetDevice();
		BV_ASSERT(pDevice != nullptr, "GpuBuffer::Create => D3D Device not initialized or assigned");

		GraphicsResult result = GraphicsResult::SUCCESS;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = usage;
		bufferDesc.BindFlags = bindFlags;
		bufferDesc.CPUAccessFlags = accessFlags;
		bufferDesc.MiscFlags = miscFlags;
		bufferDesc.StructureByteStride = structureByteStride;
		if (D3D11_BIND_CONSTANT_BUFFER == bindFlags)
		{
			bufferDesc.ByteWidth = bufferSize % 16 == 0 ? bufferSize : bufferSize + (16 - (bufferSize % 16));
		}
		else
		{
			bufferDesc.ByteWidth = bufferSize;
		}


		if (pData)
		{
			D3D11_SUBRESOURCE_DATA bufferData = { 0 };
			bufferData.pSysMem = pData;

			result = Create(bufferDesc, &bufferData);
		}
		else
		{
			result = Create(bufferDesc);
		}

		return result;
	}


	GraphicsResult GpuBuffer::Create(const D3D11_BUFFER_DESC & bufferDesc, D3D11_SUBRESOURCE_DATA * const pData)
	{
		return SUCCEEDED(RendererD3D11::GetMainInstance()->GetDevice()->CreateBuffer(&bufferDesc, pData, m_pBuffer.ReleaseAndGetAddressOf())) ?
			GraphicsResult::SUCCESS : GraphicsResult::HARDWARE_BUFFER_FAIL;
	}

	void GpuBuffer::Release()
	{
		if (m_pBuffer)
		{
			m_pBuffer.Reset();
			m_pBuffer = nullptr;
		}
	}


	GraphicsResult GpuBuffer::Map(void * const pData, const size_t bufferSize)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		ID3D11DeviceContext *pContext = RendererD3D11::GetMainInstance()->GetContext();
		BV_ASSERT(pContext != nullptr, "GpuBuffer::Map => D3D Device Context not initialized or assigned");

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pContext->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			result = GraphicsResult::HARDWARE_BUFFER_FAIL;
		}
		else
		{
			CopyMemory(mappedResource.pData, pData, bufferSize);
			pContext->Unmap(m_pBuffer.Get(), 0);
		}

		return result;
	}
}