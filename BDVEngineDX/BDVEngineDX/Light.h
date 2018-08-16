#ifndef LIGHT_H
#define LIGHT_H

#include "Utils.h"

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular;

	// Directional lights only have a directional and are assumed to be infinitely far away
	XMFLOAT3 m_Direction;
	float pad;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular;

	// Point lights doesn't have a direction; they send light rays to everything around its own position, but have a range indicating the intensity
	// There's also the attenuation values, which is the inverse of a squared function used to represent the amount of light faded,
	// according to the range
	XMFLOAT3 m_Position;
	float m_Range;
	XMFLOAT3 m_Attenuation;
	float pad;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular;

	// Spot lights have a position, as well as a direction to which its cone points to
	// Range and attenuation values behave the same way
	XMFLOAT3 m_Position;
	float m_Range;

	XMFLOAT3 m_Direction;
	float m_Spot;

	XMFLOAT3 m_Attenuation;
	float pad;
};

#endif