#include "IndexBuffer.h"
#include "IndexBuffer.h"


namespace BVGraphics
{
	IndexBuffer::IndexBuffer()
		: m_IndexSize(0),m_NumIndices(0)
	{
	}


	IndexBuffer::~IndexBuffer()
	{
	}


	GraphicsResult IndexBuffer::Create(void * const pIndices, const unsigned int numIndices, const unsigned int indexSize, const D3D11_USAGE usage, const unsigned int accessFlags, const unsigned int miscFlags, const unsigned int structureByteStride)
	{
		m_IndexSize = indexSize;
		m_NumIndices = numIndices;

		return GpuBuffer::Create(pIndices, numIndices * indexSize, usage, D3D11_BIND_INDEX_BUFFER, accessFlags, miscFlags, structureByteStride);
	}
}