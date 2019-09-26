#include "StructuredBuffer.h"


namespace BVGraphics
{
	StructuredBuffer::StructuredBuffer()
		: m_pSRV(nullptr)
	{
	}


	StructuredBuffer::~StructuredBuffer()
	{
		Release();

		if (m_pSRV)
		{
			delete m_pSRV;
		}
	}


	GraphicsResult BVGraphics::StructuredBuffer::CreateDynamic(const unsigned int numElements, const unsigned int elementSize)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		do
		{
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = GpuBuffer::Create(nullptr, numElements * elementSize, D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE,
				D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, elementSize)))
			{
				break;
			}

			m_pSRV = new ShaderResourceView();
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			shaderResourceViewDesc.Buffer.ElementWidth = numElements;
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pSRV->Create(m_pBuffer.Get(), shaderResourceViewDesc)))
			{
				delete m_pSRV;
				m_pSRV = nullptr;
				break;
			}
		} while (0);

		return result;
	}

	GraphicsResult StructuredBuffer::Create(const D3D11_BUFFER_DESC & bufferDesc, D3D11_SUBRESOURCE_DATA * const pData, const D3D11_SHADER_RESOURCE_VIEW_DESC & shaderResourceViewDesc)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		do
		{
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = GpuBuffer::Create(bufferDesc, pData)))
			{
				break;
			}

			m_pSRV = new ShaderResourceView();
			if (GraphicsResult::HARDWARE_BUFFER_FAIL == (result = m_pSRV->Create(m_pBuffer.Get(), shaderResourceViewDesc)))
			{
				delete m_pSRV;
				m_pSRV = nullptr;
				break;
			}
		} while (0);

		return result;
	}
	void StructuredBuffer::Release()
	{
		if (m_pSRV)
		{
			m_pSRV->Release();
		}

		GpuBuffer::Release();
	}
}