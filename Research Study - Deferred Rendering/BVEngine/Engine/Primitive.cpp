#include "Primitive.h"
#include "MemoryUtils.h"
#include "BVVector_SIMD.h"

using namespace BVMath::SIMD;

namespace BVEngine
{
	Primitive::Primitive(Allocator * const pAllocator)
		: m_pAllocator(pAllocator), m_pVertices(nullptr), m_NumVertices(0), m_pIndices(nullptr), m_NumIndices(0)
	{
	}


	Primitive::~Primitive()
	{
		Clear();
		m_pAllocator = nullptr;
	}


	void Primitive::Clear()
	{
		if (m_pVertices && m_pIndices)
		{
			if (m_pAllocator)
			{
				m_pAllocator->FreeU(m_pVertices);
				m_pAllocator->FreeU(m_pIndices);
			}
			else
			{
				delete[](m_pVertices);
				delete[](m_pIndices);
			}
		}
	}

	void Primitive::CreateBox(const float x, const float y, const float z)
	{
		Clear();

		m_NumVertices = 24;
		m_NumIndices = 36;

		if (m_pAllocator)
		{
			void *pMem = m_pAllocator->AllocateU(sizeof(Vertex3D) * m_NumVertices);
			m_pVertices = reinterpret_cast<Vertex3D *>(pMem);
			pMem = m_pAllocator->AllocateU(sizeof(unsigned int) * m_NumIndices);
			m_pIndices = reinterpret_cast<unsigned int *>(pMem);
		}
		else
		{
			m_pVertices = new Vertex3D[m_NumVertices];
			m_pIndices = new unsigned int[m_NumIndices];
		}

		float w = 0.5f * x;
		float h = 0.5f * y;
		float d = 0.5f * z;

		m_pVertices[0]  = Vertex3D(BVFloat3(-w, -h, -d), BVFloat2(0.0f, 1.0f), BVFloat3( 0.0f,  0.0f, -1.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[1]  = Vertex3D(BVFloat3(-w, +h, -d), BVFloat2(0.0f, 0.0f), BVFloat3( 0.0f,  0.0f, -1.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[2]  = Vertex3D(BVFloat3(+w, +h, -d), BVFloat2(1.0f, 0.0f), BVFloat3( 0.0f,  0.0f, -1.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[3]  = Vertex3D(BVFloat3(+w, -h, -d), BVFloat2(1.0f, 1.0f), BVFloat3( 0.0f,  0.0f, -1.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[4]  = Vertex3D(BVFloat3(-w, -h, +d), BVFloat2(1.0f, 1.0f), BVFloat3( 0.0f,  0.0f,  1.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[5]  = Vertex3D(BVFloat3(+w, -h, +d), BVFloat2(0.0f, 1.0f), BVFloat3( 0.0f,  0.0f,  1.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[6]  = Vertex3D(BVFloat3(+w, +h, +d), BVFloat2(0.0f, 0.0f), BVFloat3( 0.0f,  0.0f,  1.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[7]  = Vertex3D(BVFloat3(-w, +h, +d), BVFloat2(1.0f, 0.0f), BVFloat3( 0.0f,  0.0f,  1.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[8]  = Vertex3D(BVFloat3(-w, +h, -d), BVFloat2(0.0f, 1.0f), BVFloat3( 0.0f,  1.0f,  0.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[9]  = Vertex3D(BVFloat3(-w, +h, +d), BVFloat2(0.0f, 0.0f), BVFloat3( 0.0f,  1.0f,  0.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[10] = Vertex3D(BVFloat3(+w, +h, +d), BVFloat2(1.0f, 0.0f), BVFloat3( 0.0f,  1.0f,  0.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[11] = Vertex3D(BVFloat3(+w, +h, -d), BVFloat2(1.0f, 1.0f), BVFloat3( 0.0f,  1.0f,  0.0f), BVFloat3( 1.0f, 0.0f,  0.0f));
		m_pVertices[12] = Vertex3D(BVFloat3(-w, -h, -d), BVFloat2(1.0f, 1.0f), BVFloat3( 0.0f, -1.0f,  0.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[13] = Vertex3D(BVFloat3(+w, -h, -d), BVFloat2(0.0f, 1.0f), BVFloat3( 0.0f, -1.0f,  0.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[14] = Vertex3D(BVFloat3(+w, -h, +d), BVFloat2(0.0f, 0.0f), BVFloat3( 0.0f, -1.0f,  0.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[15] = Vertex3D(BVFloat3(-w, -h, +d), BVFloat2(1.0f, 0.0f), BVFloat3( 0.0f, -1.0f,  0.0f), BVFloat3(-1.0f, 0.0f,  0.0f));
		m_pVertices[16] = Vertex3D(BVFloat3(-w, -h, +d), BVFloat2(0.0f, 1.0f), BVFloat3(-1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f, -1.0f));
		m_pVertices[17] = Vertex3D(BVFloat3(-w, +h, +d), BVFloat2(0.0f, 0.0f), BVFloat3(-1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f, -1.0f));
		m_pVertices[18] = Vertex3D(BVFloat3(-w, +h, -d), BVFloat2(1.0f, 0.0f), BVFloat3(-1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f, -1.0f));
		m_pVertices[19] = Vertex3D(BVFloat3(-w, -h, -d), BVFloat2(1.0f, 1.0f), BVFloat3(-1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f, -1.0f));
		m_pVertices[20] = Vertex3D(BVFloat3(+w, -h, -d), BVFloat2(0.0f, 1.0f), BVFloat3( 1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f,  1.0f));
		m_pVertices[21] = Vertex3D(BVFloat3(+w, +h, -d), BVFloat2(0.0f, 0.0f), BVFloat3( 1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f,  1.0f));
		m_pVertices[22] = Vertex3D(BVFloat3(+w, +h, +d), BVFloat2(1.0f, 0.0f), BVFloat3( 1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f,  1.0f));
		m_pVertices[23] = Vertex3D(BVFloat3(+w, -h, +d), BVFloat2(1.0f, 1.0f), BVFloat3( 1.0f,  0.0f,  0.0f), BVFloat3( 0.0f, 0.0f,  1.0f));

		m_pIndices[0]  = 0;  m_pIndices[1]  = 1;  m_pIndices[2]  = 2;
		m_pIndices[3]  = 0;  m_pIndices[4]  = 2;  m_pIndices[5]  = 3;
		m_pIndices[6]  = 4;  m_pIndices[7]  = 5;  m_pIndices[8]  = 6;
		m_pIndices[9]  = 4;  m_pIndices[10] = 6;  m_pIndices[11] = 7;
		m_pIndices[12] = 8;  m_pIndices[13] = 9;  m_pIndices[14] = 10;
		m_pIndices[15] = 8;  m_pIndices[16] = 10; m_pIndices[17] = 11;
		m_pIndices[18] = 12; m_pIndices[19] = 13; m_pIndices[20] = 14;
		m_pIndices[21] = 12; m_pIndices[22] = 14; m_pIndices[23] = 15;
		m_pIndices[24] = 16; m_pIndices[25] = 17; m_pIndices[26] = 18;
		m_pIndices[27] = 16; m_pIndices[28] = 18; m_pIndices[29] = 19;
		m_pIndices[30] = 20; m_pIndices[31] = 21; m_pIndices[32] = 22;
		m_pIndices[33] = 20; m_pIndices[34] = 22; m_pIndices[35] = 23;
	}

	void Primitive::CreateGeosphere(const float radius, size_t numSubdivisions)
	{
		Clear();

		const size_t vertexCount[] = { 12, 120, 480, 1920, 7680, 30720 };
		const size_t indexCount[] = { 60, 240, 960, 3840, 15360, 61440 };

		if (numSubdivisions > 5)
		{
			numSubdivisions = 5;
		}

		m_NumVertices = vertexCount[numSubdivisions];
		m_NumIndices = indexCount[numSubdivisions];

		Vertex3D *pTmpVertices = nullptr;
		unsigned int *pTmpIndices = nullptr;

		if (m_pAllocator)
		{
			void *pMem = m_pAllocator->AllocateU(sizeof(Vertex3D) * m_NumVertices);
			m_pVertices = reinterpret_cast<Vertex3D *>(pMem);
			pMem = m_pAllocator->AllocateU(sizeof(unsigned int) * m_NumIndices);
			m_pIndices = reinterpret_cast<unsigned int *>(pMem);

			pMem = m_pAllocator->AllocateU(sizeof(Vertex3D) * m_NumVertices);
			pTmpVertices = reinterpret_cast<Vertex3D *>(pMem);
			pMem = m_pAllocator->AllocateU(sizeof(unsigned int) * m_NumIndices);
			pTmpIndices = reinterpret_cast<unsigned int *>(pMem);
		}
		else
		{
			m_pVertices = new Vertex3D[m_NumVertices];
			m_pIndices = new unsigned int[m_NumIndices];

			pTmpVertices = new Vertex3D[m_NumVertices];
			pTmpIndices = new unsigned int[m_NumIndices];
		}

		const float x = 0.525731f;
		const float z = 0.850651f;

		BVFloat3 vertices[12] =
		{
			BVFloat3(-x, 0.0f, z),  BVFloat3(x, 0.0f, z),
			BVFloat3(-x, 0.0f, -z), BVFloat3(x, 0.0f, -z),
			BVFloat3(0.0f, z, x),   BVFloat3(0.0f, z, -x),
			BVFloat3(0.0f, -z, x),  BVFloat3(0.0f, -z, -x),
			BVFloat3(z, x, 0.0f),   BVFloat3(-z, x, 0.0f),
			BVFloat3(z, -x, 0.0f),  BVFloat3(-z, -x, 0.0f)
		};

		unsigned int indices[60] =
		{
			1,  4,  0,   4, 9, 0,  4, 5,  9,  8, 5, 4,   1,  8, 4,
			1,  10, 8,  10, 3, 8,  8, 3,  5,  3, 2, 5,   3,  7, 2,
			3,  10, 7,  10, 6, 7,  6, 11, 7,  6, 0, 11,  6,  1, 0,
			10, 1,  6,  11, 0, 9,  2, 11, 9,  5, 2, 9,   11, 2, 7
		};

		size_t i, j;

		for (i = 0; i < 12; i++)
		{
			m_pVertices[i].m_Position = vertices[i];
		}

		for (i = 0; i < 60; i++)
		{
			m_pIndices[i] = indices[i];
		}

		size_t currVertPos;
		size_t currIndexPos;
		size_t numTriangles = 20;
		size_t numVertices = 30; // used to match the subsequent number of vertices on subdivisions

		Vertex3D v0, v1, v2;
		Vertex3D m0, m1, m2;
		for (i = 0; i < numSubdivisions; i++)
		{
			CopyMemory(pTmpVertices, m_pVertices, numVertices * sizeof(Vertex3D));
			CopyMemory(pTmpIndices, m_pIndices, numTriangles * 3 * sizeof(unsigned int));

			currVertPos = 0;
			currIndexPos = 0;

			for (j = 0; j < numTriangles; j++)
			{
				v0 = pTmpVertices[pTmpIndices[j * 3 + 0]];
				v1 = pTmpVertices[pTmpIndices[j * 3 + 1]];
				v2 = pTmpVertices[pTmpIndices[j * 3 + 2]];

				m0.m_Position = BVFloat3(
					0.5f * (v0.m_Position.x + v1.m_Position.x),
					0.5f * (v0.m_Position.y + v1.m_Position.y),
					0.5f * (v0.m_Position.z + v1.m_Position.z));

				m1.m_Position = BVFloat3(
					0.5f * (v1.m_Position.x + v2.m_Position.x),
					0.5f * (v1.m_Position.y + v2.m_Position.y),
					0.5f * (v1.m_Position.z + v2.m_Position.z));

				m2.m_Position = BVFloat3(
					0.5f * (v0.m_Position.x + v2.m_Position.x),
					0.5f * (v0.m_Position.y + v2.m_Position.y),
					0.5f * (v0.m_Position.z + v2.m_Position.z));

				m_pVertices[currVertPos++] = v0;
				m_pVertices[currVertPos++] = v1;
				m_pVertices[currVertPos++] = v2;
				m_pVertices[currVertPos++] = m0;
				m_pVertices[currVertPos++] = m1;
				m_pVertices[currVertPos++] = m2;

				m_pIndices[currIndexPos++] = j * 6 + 0;
				m_pIndices[currIndexPos++] = j * 6 + 3;
				m_pIndices[currIndexPos++] = j * 6 + 5;

				m_pIndices[currIndexPos++] = j * 6 + 3;
				m_pIndices[currIndexPos++] = j * 6 + 4;
				m_pIndices[currIndexPos++] = j * 6 + 5;

				m_pIndices[currIndexPos++] = j * 6 + 5;
				m_pIndices[currIndexPos++] = j * 6 + 4;
				m_pIndices[currIndexPos++] = j * 6 + 2;

				m_pIndices[currIndexPos++] = j * 6 + 3;
				m_pIndices[currIndexPos++] = j * 6 + 1;
				m_pIndices[currIndexPos++] = j * 6 + 4;
			}

			numTriangles *= 4;
			numVertices *= 4;
		}

		if (m_pAllocator)
		{
			m_pAllocator->FreeU(pTmpVertices);
			m_pAllocator->FreeU(pTmpIndices);
		}
		else
		{
			delete[] pTmpVertices;
			delete[] pTmpIndices;
		}

		for (i = 0; i < m_NumVertices; ++i)
		{
			// Project onto unit sphere.
			BVVector n = VectorNormalize(VectorFromFloat3(m_pVertices[i].m_Position));

			// Project onto sphere.
			BVVector p = radius * n;

			VectorToFloat3(p, m_pVertices[i].m_Position);
			VectorToFloat3(n, m_pVertices[i].m_Normal);

			// Derive texture coordinates from spherical coordinates.
			float theta = atan2f(m_pVertices[i].m_Position.z, m_pVertices[i].m_Position.x);
			if (theta < 0.0f)
			{
				theta += TWO_TIMES_PI;
			}

			float phi = acosf(m_pVertices[i].m_Position.y / radius);

			m_pVertices[i].m_Texture.x = theta * ONE_OVER_TWO_TIMES_PI;
			m_pVertices[i].m_Texture.y = phi * ONE_OVER_PI;

			// Partial derivative of P with respect to theta
			m_pVertices[i].m_Tangent.x = -radius * sinf(phi) * sinf(theta);
			m_pVertices[i].m_Tangent.y = 0.0f;
			m_pVertices[i].m_Tangent.z = +radius * sinf(phi) * cosf(theta);

			BVVector t = VectorFromFloat3(m_pVertices[i].m_Tangent);
			VectorToFloat3(VectorNormalize(t), m_pVertices[i].m_Tangent);
		}
	}

	void Primitive::CreateCone(const unsigned int numSlices, const unsigned int numStacks, const float height, const float radius, bool center)
	{
		CreateCylinder(numSlices, numStacks, height, 0.0f, radius, center);
	}

	void Primitive::CreateCylinder(const unsigned int numSlices, const unsigned int numStacks, const float height,
		const float topRadius, const float bottomRadius, bool center)
	{
		Clear();

		m_NumVertices = (numStacks + 1) * (numSlices + 1) + (numSlices + 2) * 2;
		m_NumIndices = numSlices * 6 * (numStacks + 1);

		if (m_pAllocator)
		{
			void *pMem = m_pAllocator->AllocateU(sizeof(Vertex3D) * m_NumVertices);
			m_pVertices = reinterpret_cast<Vertex3D *>(pMem);
			pMem = m_pAllocator->AllocateU(sizeof(unsigned int) * m_NumIndices);
			m_pIndices = reinterpret_cast<unsigned int *>(pMem);
		}
		else
		{
			m_pVertices = new Vertex3D[m_NumVertices];
			m_pIndices = new unsigned int[m_NumIndices];
		}

		float stackHeight = height / numStacks;

		// Amount to increment radius as we move up each stack level from bottom to top.
		float radiusStep = (topRadius - bottomRadius) / numStacks;

		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;

		float dTheta = TWO_TIMES_PI / numSlices;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (unsigned int i = 0; i <= numStacks; ++i)
		{
			//float y = -0.5f * height + i * stackHeight;
			float y = -height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			// vertices of ring
			for (unsigned int j = 0; j <= numSlices; j++)
			{
				Vertex3D vertex;

				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);

				vertex.m_Position = BVFloat3(r * c, y, r * s);

				vertex.m_Texture.x = (float)j / numSlices;
				vertex.m_Texture.y = 1.0f - (float)i / numStacks;

				vertex.m_Tangent = BVFloat3(-s, 0.0f, c);

				float dr = bottomRadius - topRadius;
				BVFloat3 bitangent(dr * c, -height, dr * s);

				BVVector normal = VectorNormalize(VectorCross(VectorFromFloat3(vertex.m_Tangent), VectorFromFloat3(bitangent)));
				VectorToFloat3(normal, vertex.m_Normal);

				m_pVertices[vertexCount++] = vertex;
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		unsigned int ringVertexCount = numSlices + 1;

		// Compute indices for each stack.
		for (unsigned int i = 0; i < numStacks; ++i)
		{
			for (unsigned int j = 0; j < numSlices; ++j)
			{
				m_pIndices[indexCount++] = i * ringVertexCount + j;
				m_pIndices[indexCount++] = (i + 1) * ringVertexCount + j;
				m_pIndices[indexCount++] = (i + 1) * ringVertexCount + j + 1;

				m_pIndices[indexCount++] = i * ringVertexCount + j;
				m_pIndices[indexCount++] = (i + 1) * ringVertexCount + j + 1;
				m_pIndices[indexCount++] = i * ringVertexCount + j + 1;
			}
		}

		// Top Cap
		unsigned int baseIndex = vertexCount;

		float y = center ? 0.5f * height : 0.0f;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (unsigned int i = 0; i <= numSlices; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			m_pVertices[vertexCount++] = Vertex3D(BVFloat3(x, y, z), BVFloat2(u, v), BVFloat3(0.0f, 1.0f, 0.0f), BVFloat3(1.0f, 0.0f, 0.0f));
		}

		// Cap center vertex.
		m_pVertices[vertexCount++] = Vertex3D(BVFloat3(0.0f, y, 0.0f), BVFloat2(0.5f, 0.5f), BVFloat3(0.0f, 1.0f, 0.0f), BVFloat3(1.0f, 0.0f, 0.0f));

		// Index of center vertex.
		unsigned int centerIndex = vertexCount - 1;

		for (unsigned int i = 0; i < numSlices; ++i)
		{
			m_pIndices[indexCount++] = centerIndex;
			m_pIndices[indexCount++] = baseIndex + i + 1;
			m_pIndices[indexCount++] = baseIndex + i;
		}

		// Bottom Cap
		baseIndex = vertexCount;

		y = center ? -0.5f * height : -height;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (unsigned int i = 0; i <= numSlices; ++i)
		{
			float x = bottomRadius * cosf(i * dTheta);
			float z = bottomRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			m_pVertices[vertexCount++] = Vertex3D(BVFloat3(x, y, z), BVFloat2(u, v), BVFloat3(0.0f, -1.0f, 0.0f), BVFloat3(1.0f, 0.0f, 0.0f));
		}

		// Cap center vertex.
		m_pVertices[vertexCount++] = Vertex3D(BVFloat3(0.0f, y, 0.0f), BVFloat2(0.5f, 0.5f), BVFloat3(0.0f, -1.0f, 0.0f), BVFloat3(1.0f, 0.0f, 0.0f));

		// Index of center vertex.
		centerIndex = vertexCount - 1;

		for (unsigned int i = 0; i < numSlices; ++i)
		{
			m_pIndices[indexCount++] = centerIndex;
			m_pIndices[indexCount++] = baseIndex + i;
			m_pIndices[indexCount++] = baseIndex + i + 1;
		}
	}

	void Primitive::CreateGrid(const float width, const float depth, const unsigned int numRows, const unsigned int numCols)
	{
		Clear();

		m_NumVertices = (numRows + 1) * (numCols + 1);
		m_NumIndices = numRows * numCols * 6;

		if (m_pAllocator)
		{
			void *pMem = m_pAllocator->AllocateU(sizeof(Vertex3D) * m_NumVertices);
			m_pVertices = reinterpret_cast<Vertex3D *>(pMem);
			pMem = m_pAllocator->AllocateU(sizeof(unsigned int) * m_NumIndices);
			m_pIndices = reinterpret_cast<unsigned int *>(pMem);
		}
		else
		{
			m_pVertices = new Vertex3D[m_NumVertices];
			m_pIndices = new unsigned int[m_NumIndices];
		}

		float xStep = ((float)numCols * width) * 0.5f;
		float zStep = ((float)numRows * depth) * 0.5f;

		float currXStep = -xStep;
		float currZStep = zStep;

		size_t i, j;
		Vertex3D v;
		v.m_Normal = BVFloat3(0.0f, 1.0f, 0.0f);
		v.m_Tangent = BVFloat3(1.0f, 0.0f, 0.0f);

		size_t currIndex = 0;
		size_t currVertex = 0;
		float texX, texY;
		for (i = 0; i <= numRows; i++)
		{
			currXStep = -xStep;

			texY = (float)i / (float)numRows;
			for (j = 0; j <= numCols; j++)
			{
				texX = (float)j / (float)numCols;

				v.m_Position = BVFloat3(currXStep, 0.0f, currZStep);
				v.m_Texture = BVFloat2(texX, texY);
				m_pVertices[currVertex++] = v;

				currXStep += width;

				if (j < numCols && i < numRows)
				{
					m_pIndices[currIndex++] = i * (numCols + 1) + j;
					m_pIndices[currIndex++] = i * (numCols + 1) + j + 1;
					m_pIndices[currIndex++] = (i + 1) * (numCols + 1) + j + 1;

					m_pIndices[currIndex++] = i * (numCols + 1) + j;
					m_pIndices[currIndex++] = (i + 1) * (numCols + 1) + j + 1;
					m_pIndices[currIndex++] = (i + 1) * (numCols + 1) + j;
				}
			}
			currZStep -= depth;
		}
	}
}