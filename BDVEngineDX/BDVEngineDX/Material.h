#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular; // w = SpecPower
	XMFLOAT4 m_Reflect;
	XMFLOAT4 m_Color;
};

#endif