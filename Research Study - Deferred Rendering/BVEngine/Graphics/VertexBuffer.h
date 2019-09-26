#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "GpuBuffer.h"

namespace BVGraphics
{
	class VertexBuffer : public GpuBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		GraphicsResult CreateDefault(void * const pVertices, const unsigned int numVertices, const unsigned int vertexSize);
		GraphicsResult CreateDynamic(const unsigned int numVertices, const unsigned int vertexSize);
		GraphicsResult Create(void * const pData, const unsigned int numVertices, const unsigned int vertexSize,
			const D3D11_USAGE usage = D3D11_USAGE_DEFAULT, const unsigned int accessFlags = 0,
			const unsigned int miscFlags = 0, const unsigned int structureByteStride = 0);

		inline unsigned int GetVertexSize() const { return m_VertexSize; }
		inline unsigned int GetNumVertices() const { return m_NumVertices; }

	private:
		VertexBuffer(const VertexBuffer & other) = delete;
		VertexBuffer(VertexBuffer && other) = delete;
		VertexBuffer & operator = (const VertexBuffer & other) = delete;
		VertexBuffer & operator = (VertexBuffer && other) = delete;

	private:
		unsigned int m_VertexSize;
		unsigned int m_NumVertices;
	};
}

#endif