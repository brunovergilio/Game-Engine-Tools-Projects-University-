#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "BVMath.h"

using namespace BVMath;

class BoundingSphere
{
public:
	BoundingSphere();
	~BoundingSphere();

	BVFloat3 m_Center;
	float m_Radius;
};

#endif