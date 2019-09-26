#ifndef MATERIAL_H
#define MATERIAL_H

#include "GraphicsUtils.h"
#include "BVMath.h"

using namespace BVMath;

namespace BVEngine
{
	struct Material
	{
	public:
		Material() { ZeroMemory(this, sizeof(Material)); }

		BVFloat4 m_HemsTopColor;
		BVFloat4 m_HemsBottomColor;
		BVFloat4 m_Diffuse;
		BVFloat4 m_Specular; // W is for specular power
		int m_UseColor;
		int m_UseTexture;
		BVFloat2 m_Pad;
	};
}

#endif