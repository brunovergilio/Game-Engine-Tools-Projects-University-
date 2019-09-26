#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "GpuBuffer.h"

namespace BVGraphics
{
	class IndexBuffer : public GpuBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		GraphicsResult Create(void * const pIndices, const unsigned int numIndices, const unsigned int indexSize = sizeof(unsigned int),
			const D3D11_USAGE usage = D3D11_USAGE_DEFAULT, const unsigned int accessFlags = 0,
			const unsigned int miscFlags = 0, const unsigned int structureByteStride = 0);

		inline unsigned int GetIndexSize() const { return m_IndexSize; }
		inline unsigned int GetNumIndices() const { return m_NumIndices; }

	private:
		IndexBuffer(const IndexBuffer & other) = delete;
		IndexBuffer(IndexBuffer && other) = delete;
		IndexBuffer & operator = (const IndexBuffer & other) = delete;
		IndexBuffer & operator = (IndexBuffer && other) = delete;

	private:
		unsigned int m_IndexSize;
		unsigned int m_NumIndices;
	};
}

#endif