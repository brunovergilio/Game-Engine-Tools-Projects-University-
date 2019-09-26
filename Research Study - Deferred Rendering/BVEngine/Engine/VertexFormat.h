#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include <BVMath.h>
#include "GraphicsUtils.h"

using namespace BVMath;

struct Vertex2D
{
	BVFloat3 m_Position;
	BVFloat2 m_Texture;
	Vertex2D() { ZeroMemory(this, sizeof(Vertex2D)); }
	Vertex2D(const BVFloat3 & position, const BVFloat2 & texture)
		: m_Position(position), m_Texture(texture) { }

	static D3D11_INPUT_ELEMENT_DESC m_InputLayout[];
	static size_t m_InputLayoutSize;
	static size_t m_VertexSize;
};

struct Vertex3D
{
	BVFloat3 m_Position;
	BVFloat2 m_Texture;
	BVFloat3 m_Normal;
	BVFloat3 m_Tangent;
	Vertex3D() { ZeroMemory(this, sizeof(Vertex3D)); }
	Vertex3D(const BVFloat3 & position, const BVFloat2 & texture, const BVFloat3 & normal, const BVFloat3 & tangent)
		: m_Position(position), m_Texture(texture), m_Normal(normal), m_Tangent(tangent) { }

	static D3D11_INPUT_ELEMENT_DESC m_InputLayout[];
	static D3D11_INPUT_ELEMENT_DESC m_InputLayoutInstanced[];
	static size_t m_InputLayoutSize;
	static size_t m_InputLayoutInstancedSize;
	static size_t m_VertexSize;
};

struct Vertex3DLightVolume
{
	BVFloat3 m_Position;
	Vertex3DLightVolume() { ZeroMemory(this, sizeof(Vertex3DLightVolume)); }
	Vertex3DLightVolume(const BVFloat3 & position)
		: m_Position(position) { }

	static D3D11_INPUT_ELEMENT_DESC m_InputLayout[];
	static D3D11_INPUT_ELEMENT_DESC m_InputLayoutInstanced[];
	static size_t m_InputLayoutSize;
	static size_t m_InputLayoutInstancedSize;
	static size_t m_VertexSize;
};

struct Vertex3DAnim
{
	BVFloat3 m_Position;
	BVFloat2 m_Texture;
	BVFloat3 m_Normal;
	BVFloat3 m_Tangent;
	unsigned int m_Joints[4];
	float m_Weights[4];
	Vertex3DAnim() { ZeroMemory(this, sizeof(Vertex3DAnim)); }
	Vertex3DAnim(const BVFloat3 & position, const BVFloat2 & texture, const BVFloat3 & normal, const BVFloat3 & tangent,
		const unsigned int * const pJoints, const float * const pWeights)
		: m_Position(position), m_Texture(texture), m_Normal(normal), m_Tangent(tangent)
	{
		CopyMemory(m_Joints, pJoints, sizeof(m_Joints));
		CopyMemory(m_Weights, pWeights, sizeof(m_Weights));
	}

	static D3D11_INPUT_ELEMENT_DESC m_InputLayout[];
	static size_t m_InputLayoutSize;
	static size_t m_VertexSize;
};

#endif