#ifndef GPUBUFFER_H
#define GPUBUFFER_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class GpuBuffer
	{
	public:
		GpuBuffer();
		virtual ~GpuBuffer();

		GraphicsResult Create(void * const pData, const unsigned int bufferSize,
			const D3D11_USAGE usage, const unsigned int bindFlags, const unsigned int accessFlags = 0,
			const unsigned int miscFlags = 0, const unsigned int structureByteStride = 0);

		GraphicsResult Create(const D3D11_BUFFER_DESC & bufferDesc, D3D11_SUBRESOURCE_DATA * const pData = nullptr);

		virtual void Release();

		GraphicsResult Map(void * const pData, const size_t bufferSize);

		inline ID3D11Buffer * const Get() const { return m_pBuffer.Get(); }

	private:
		GpuBuffer(const GpuBuffer & other) = delete;
		GpuBuffer(GpuBuffer && other) = delete;
		GpuBuffer & operator = (const GpuBuffer & other) = delete;
		GpuBuffer & operator = (GpuBuffer && other) = delete;

	protected:
		ComPtr<ID3D11Buffer> m_pBuffer;
	};
}

#endif