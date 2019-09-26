#ifndef STRUCTUREDBUFFER_H
#define STRUCTUREDBUFFER_H

#include "GpuBuffer.h"
#include "ShaderResourceView.h"

namespace BVGraphics
{
	class StructuredBuffer : public GpuBuffer
	{
	public:
		StructuredBuffer();
		~StructuredBuffer();

		GraphicsResult CreateDynamic(const unsigned int numElements, const unsigned int elementSize);
		GraphicsResult Create(const D3D11_BUFFER_DESC & bufferDesc, D3D11_SUBRESOURCE_DATA * const pData,
			const D3D11_SHADER_RESOURCE_VIEW_DESC & shaderResourceViewDesc);

		void Release() override;

		inline ShaderResourceView * const GetSRV() const { return m_pSRV; }

	private:
		StructuredBuffer(const StructuredBuffer & other) = delete;
		StructuredBuffer(StructuredBuffer && other) = delete;
		StructuredBuffer & operator = (const StructuredBuffer & other) = delete;
		StructuredBuffer & operator = (StructuredBuffer && other) = delete;

	private:
		ShaderResourceView *m_pSRV;
	};
}

#endif