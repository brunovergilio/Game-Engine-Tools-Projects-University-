#include "ConstantBuffer.h"


namespace BVGraphics
{
	ConstantBuffer::ConstantBuffer()
	{
	}


	ConstantBuffer::~ConstantBuffer()
	{
	}


	GraphicsResult ConstantBuffer::CreateFixed(void * const pData, const unsigned int size)
	{
		return GpuBuffer::Create(pData, size, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER);
	}

	GraphicsResult ConstantBuffer::CreateDynamic(const unsigned int size)
	{
		return GpuBuffer::Create(nullptr, size, D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);
	}

	GraphicsResult ConstantBuffer::Create(void * const pData, const unsigned int size, const D3D11_USAGE usage, const unsigned int accessFlags, const unsigned int miscFlags, const unsigned int structureByteStride)
	{
		return GpuBuffer::Create(pData, size, usage, D3D11_BIND_CONSTANT_BUFFER, accessFlags, miscFlags, structureByteStride);
	}
}