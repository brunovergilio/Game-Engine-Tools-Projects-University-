#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "Allocator.h"
#include "VertexFormat.h"

using namespace BVMemory;

namespace BVEngine
{
	class Primitive 
	{
	public:
		Primitive(Allocator * const pAllocator = nullptr);
		~Primitive();
		void CreateBox(const float x, const float y, const float z);
		void CreateGeosphere(const float radius, size_t numSubdivisions);
		void CreateCone(const unsigned int numSlices, const unsigned int numStacks, const float height, const float radius, bool center = true);
		void CreateCylinder(const unsigned int numSlices, const unsigned int numStacks, const float height,
			const float topRadius, const float bottomRadius, bool center = true);
		void CreateGrid(const float width, const float depth, const unsigned int numRows, const unsigned int numCols);

	private:
		void Clear();

		Primitive(const Primitive & other) = delete;
		Primitive(Primitive && other) = delete;
		Primitive & operator = (const Primitive & other) = delete;
		Primitive & operator = (Primitive && other) = delete;

	public:
		Vertex3D *m_pVertices;
		size_t m_NumVertices;
		unsigned int *m_pIndices;
		size_t m_NumIndices;

	private:
		Allocator * m_pAllocator;
	};
}
#endif