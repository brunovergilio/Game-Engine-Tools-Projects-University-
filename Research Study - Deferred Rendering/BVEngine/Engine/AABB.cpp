#include "AABB.h"

#include <float.h>

AABB::AABB()
{
}


AABB::~AABB()
{
}


void AABB::ComputeFromPoints(const BVFloat3 * pPoints, const unsigned int numPoints)
{
	BVFloat3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	BVFloat3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (unsigned int i = 0; i < numPoints; i++)
	{
		min.x = fminf(min.x, pPoints[i].x);
		min.y = fminf(min.y, pPoints[i].y);
		min.z = fminf(min.z, pPoints[i].z);

		max.x = fmaxf(max.x, pPoints[i].x);
		max.y = fmaxf(max.y, pPoints[i].y);
		max.z = fmaxf(max.z, pPoints[i].z);
	}
}

void AABB::ComputeFromPoints(const BVFloat3 * pPoints, const unsigned int stride, const unsigned int numPoints)
{
	BVFloat3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	BVFloat3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (unsigned int i = 0; i < numPoints; i++)
	{
		min.x = fminf(min.x, (*pPoints).x);
		min.y = fminf(min.y, (*pPoints).y);
		min.z = fminf(min.z, (*pPoints).z);

		max.x = fmaxf(max.x, (*pPoints).x);
		max.y = fmaxf(max.y, (*pPoints).y);
		max.z = fmaxf(max.z, (*pPoints).z);

		pPoints = (const BVFloat3 *)((const char *)pPoints + stride);
	}
}

void AABB::ComputeFromPoints(CBVVector * pPoints, const unsigned int numPoints)
{
	BVVector min = VectorSet(FLT_MAX, FLT_MAX, FLT_MAX);
	BVVector max = VectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (unsigned int i = 0; i < numPoints; i++)
	{
		min = VectorMin(min, pPoints[i]);
		max = VectorMax(max, pPoints[i]);
	}

	VectorToFloat3(min, m_Min);
	VectorToFloat3(max, m_Max);
}

void AABB::ComputeFromPoints(CBVVector * pPoints, const unsigned int stride, const unsigned int numPoints)
{
	BVVector min = VectorSet(FLT_MAX, FLT_MAX, FLT_MAX);
	BVVector max = VectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (unsigned int i = 0; i < numPoints; i++)
	{
		min = VectorMin(min, *pPoints);
		max = VectorMax(max, *pPoints);

		pPoints = (CBVVector *)((const char *)pPoints + stride);
	}

	VectorToFloat3(min, m_Min);
	VectorToFloat3(max, m_Max);
}
