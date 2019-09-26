#ifndef LIGHT_H
#define LIGHT_H

#include "GraphicsUtils.h"
#include "BVMath.h"

using namespace BVMath;

namespace BVEngine
{
	enum class LightType : unsigned int
	{
		Hemispheric,
		Directional,
		Point,
		Spot
	};

	struct Light
	{
	public:
		Light() { ZeroMemory(this, sizeof(Light)); }

		// 16 bytes
		BVFloat4 m_Color;

		// 16 bytes
		BVFloat3 m_Position;
		float m_Range;

		// 16 bytes
		BVFloat3 m_Direction;
		LightType m_LightType;

		// 16 bytes
		float m_CosInnerCone;
		float m_CosOuterCone;
		float m_Intensity;
		unsigned int m_CastsShadow;

		// 16 bytes
		BVFloat3 m_Attenuation; // (constant, linear, quadratic)
		unsigned int m_Enabled;
	};
}

#endif