#ifndef VERTEX_H
#define VERTEX_H

#include "Utils.h"

struct Vertex
{
	XMFLOAT3 m_Position;
	XMFLOAT2 m_Texture;
	XMFLOAT3 m_Normal;
	Vertex() { ZeroMemory(this, sizeof(Vertex)); }
	Vertex(XMFLOAT3 position, XMFLOAT2 texture, XMFLOAT3 normal)
		: m_Position(position), m_Texture(texture), m_Normal(normal) { }
};

struct Vertex2D
{
	XMFLOAT3 m_Position;
	XMFLOAT2 m_Texture;
	XMFLOAT4 m_Color;
	Vertex2D() { ZeroMemory(this, sizeof(Vertex2D)); m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); }
	Vertex2D(XMFLOAT3 position, XMFLOAT2 texture, XMFLOAT4 color)
		: m_Position(position), m_Texture(texture), m_Color(color) { }
};

#endif