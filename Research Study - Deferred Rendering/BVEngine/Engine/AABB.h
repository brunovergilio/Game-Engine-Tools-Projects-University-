#ifndef AABB_H
#define AABB_H

#include "BVVector_SIMD.h"

using namespace BVMath;
using namespace BVMath::SIMD;

class AABB
{
public:
	AABB();
	~AABB();

	void ComputeFromPoints(const BVFloat3 * pPoints, const unsigned int numPoints);
	void ComputeFromPoints(const BVFloat3 * pPoints, const unsigned int stride, const unsigned int numPoints);

	void ComputeFromPoints(CBVVector * pPoints, const unsigned int numPoints);
	void ComputeFromPoints(CBVVector * pPoints, const unsigned int stride, const unsigned int numPoints);

	BVFloat3 m_Min;
	BVFloat3 m_Max;
};

#endif