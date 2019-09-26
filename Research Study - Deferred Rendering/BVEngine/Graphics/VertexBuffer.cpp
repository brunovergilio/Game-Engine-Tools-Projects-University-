#include "VertexBuffer.h"


namespace BVGraphics
{
	VertexBuffer::VertexBuffer()
		: m_VertexSize(0), m_NumVertices(0)
	{
	}


	VertexBuffer::~VertexBuffer()
	{
	}


	GraphicsResult BVGraphics::VertexBuffer::CreateDefault(void * const pVertices, const unsigned int numVertices, const unsigned int vertexSize)
	{
		m_VertexSize = vertexSize;
		m_NumVertices = numVertices;

		return GpuBuffer::Create(pVertices, numVertices * vertexSize, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER);
	}


	GraphicsResult BVGraphics::VertexBuffer::CreateDynamic(const unsigned int numVertices, const unsigned int vertexSize)
	{
		m_VertexSize = vertexSize;
		m_NumVertices = numVertices;

		return GpuBuffer::Create(nullptr, numVertices * vertexSize, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
	}


	GraphicsResult VertexBuffer::Create(void * const pData, const unsigned int numVertices, const unsigned int vertexSize, const D3D11_USAGE usage, const unsigned int accessFlags, const unsigned int miscFlags, const unsigned int structureByteStride)
	{
		m_VertexSize = vertexSize;
		m_NumVertices = numVertices;

		return GpuBuffer::Create(pData, vertexSize * numVertices, usage, D3D11_BIND_VERTEX_BUFFER, accessFlags, miscFlags, structureByteStride);
	}
}