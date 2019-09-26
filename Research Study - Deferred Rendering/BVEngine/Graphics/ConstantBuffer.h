#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "GpuBuffer.h"

namespace BVGraphics
{
	class ConstantBuffer : public GpuBuffer
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();

		GraphicsResult CreateFixed(void * const pData, const unsigned int size);
		GraphicsResult CreateDynamic(const unsigned int size);
		GraphicsResult Create(void * const pData, const unsigned int size,
			const D3D11_USAGE usage = D3D11_USAGE_DEFAULT, const unsigned int accessFlags = 0,
			const unsigned int miscFlags = 0, const unsigned int structureByteStride = 0);

	private:
		ConstantBuffer(const ConstantBuffer & other) = delete;
		ConstantBuffer(ConstantBuffer && other) = delete;
		ConstantBuffer & operator = (const ConstantBuffer & other) = delete;
		ConstantBuffer & operator = (ConstantBuffer && other) = delete;
	};
}

#endif