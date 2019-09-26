#ifndef BVVECTOR_FPU_H
#define BVVECTOR_FPU_H

#include "BVMath.h"

namespace BVMath
{
	namespace FPU
	{
		// ==================================
		// Vector operations
		// ==================================

		// ======================
		// Declarations
		// ======================

		// ======================
		// Access
		// ======================

		BVVector VectorFromFloatPtr(const float * const p);
		BVVector VectorFromFloat2Ptr(const float * const p);
		BVVector VectorFromFloat3Ptr(const float * const p);
		BVVector VectorFromFloat4Ptr(const float * const p);

		BVVector VectorFromFloat2(const BVFloat2 & v);
		BVVector VectorFromFloat3(const BVFloat3 & v);
		BVVector VectorFromFloat4(const BVFloat4 & v);

		void VectorToFloatPtr(CBVVector v, float * const p);
		void VectorToFloat2Ptr(CBVVector v, float * const p);
		void VectorToFloat3Ptr(CBVVector v, float * const p);
		void VectorToFloat4Ptr(CBVVector v, float * const p);

		void VectorToFloat2(CBVVector v, BVFloat2 & r);
		void VectorToFloat3(CBVVector v, BVFloat3 & r);
		void VectorToFloat4(CBVVector v, BVFloat4 & r);

		float VectorGetX(CBVVector v);
		float VectorGetY(CBVVector v);
		float VectorGetZ(CBVVector v);
		float VectorGetW(CBVVector v);

		BVVector VectorSet(const float x, const float y, const float z, const float w = 0.0f);

		// ======================
		// Basic Operations
		// ======================

		BVVector VectorAdd(CBVVector v1, CBVVector v2);
		BVVector VectorSub(CBVVector v1, CBVVector v2);
		BVVector VectorMul(CBVVector v1, CBVVector v2);
		BVVector VectorDiv(CBVVector v1, CBVVector v2);

		BVVector VectorMul(CBVVector v, const float val);
		BVVector VectorDiv(CBVVector v, const float val);

		BVVector VectorMul(CBVVector v, CRBVMatrix m);

		bool VectorIsZero(CBVVector v, const float epsilon = BV_EPSILON);
		bool VectorIsEqual(CBVVector v1, CBVVector v2, const float epsilon = BV_EPSILON);

		BVVector VectorNegate(CBVVector v);
		BVVector VectorZero();
		BVVector VectorReplicate(const float s);
		BVVector VectorReplicateX(CBVVector v);
		BVVector VectorReplicateY(CBVVector v);
		BVVector VectorReplicateZ(CBVVector v);
		BVVector VectorReplicateW(CBVVector v);

		BVVector VectorMax(CBVVector v1, CBVVector v2);
		BVVector VectorMin(CBVVector v1, CBVVector v2);

		// ======================
		// Operators
		// ======================

		BVVector operator + (CBVVector v);
		BVVector operator - (CBVVector v);

		BVVector operator + (CBVVector v1, CBVVector v2);
		BVVector & operator += (RBVVector v1, CBVVector v2);

		BVVector operator - (CBVVector v1, CBVVector v2);
		BVVector & operator -= (RBVVector v1, CBVVector v2);

		BVVector operator * (CBVVector v1, CBVVector v2);
		BVVector & operator *= (RBVVector v1, CBVVector v2);
		BVVector operator * (CBVVector v, const float s);
		BVVector operator * (const float s, CBVVector v);
		BVVector & operator *= (RBVVector v, const float s);
		BVVector operator * (CBVVector v, CRBVMatrix m);
		BVVector & operator *= (RBVVector v, CRBVMatrix m);

		BVVector operator / (CBVVector v1, CBVVector v2);
		BVVector & operator /= (RBVVector v1, CBVVector v2);
		BVVector operator / (CBVVector v, const float s);
		BVVector & operator /= (RBVVector v, const float s);

		// ======================
		// Specialized Operations
		// ======================

		float VectorDot(CBVVector v1, CBVVector v2);
		BVVector VectorDotV(CBVVector v1, CBVVector v2);

		BVVector VectorCross(CBVVector v1, CBVVector v2);

		float VectorLengthSqr(CBVVector v);
		BVVector VectorLengthSqrV(CBVVector v);

		float VectorLength(CBVVector v);
		BVVector VectorLengthV(CBVVector v);

		BVVector VectorNormalize(CBVVector v);

		BVVector VectorProject(CBVVector v1, CBVVector v2);

		BVVector VectorLerp(CBVVector v1, CBVVector v2, float t);

		BVVector VectorReflect(CBVVector i, CBVVector n);

		BVVector VectorTransformDir(CBVVector v, CRBVMatrix m);
		BVVector VectorTransformPoint(CBVVector v, CRBVMatrix m);

		// =================
		// Definitions
		// =================

		// ======================
		// Access
		// ======================

		inline BVVector VectorFromFloatPtr(const float * const p)
		{
			return BVVector(p[0], 0.0f, 0.0f);
		}

		inline BVVector VectorFromFloat2Ptr(const float * const p)
		{
			return BVVector(p[0], p[1], 0.0f, 0.0f);
		}

		inline BVVector VectorFromFloat3Ptr(const float * const p)
		{
			return BVVector(p[0], p[1], p[2], 0.0f);
		}

		inline BVVector VectorFromFloat4Ptr(const float * const p)
		{
			return BVVector(p[0], p[1], p[2], p[3]);
		}

		inline BVVector VectorFromFloat2(const BVFloat2 & v)
		{
			return VectorFromFloat2Ptr(&v.x);
		}

		inline BVVector VectorFromFloat3(const BVFloat3 & v)
		{
			return VectorFromFloat3Ptr(&v.x);
		}

		inline BVVector VectorFromFloat4(const BVFloat4 & v)
		{
			return VectorFromFloat4Ptr(&v.x);
		}

		inline void VectorToFloatPtr(CBVVector v, float * const p)
		{
			p[0] = v.x;
		}

		inline void VectorToFloat2Ptr(CBVVector v, float * const p)
		{
			p[0] = v.x;
			p[1] = v.y;
		}

		inline void VectorToFloat3Ptr(CBVVector v, float * const p)
		{
			p[0] = v.x;
			p[1] = v.y;
			p[2] = v.z;
		}

		inline void VectorToFloat4Ptr(CBVVector v, float * const p)
		{
			p[0] = v.x;
			p[1] = v.y;
			p[2] = v.z;
			p[3] = v.w;
		}

		inline void VectorToFloat2(CBVVector v, BVFloat2 & r)
		{
			VectorToFloat2Ptr(v, &r.x);
		}

		inline void VectorToFloat3(CBVVector v, BVFloat3 & r)
		{
			VectorToFloat3Ptr(v, &r.x);
		}

		inline void VectorToFloat4(CBVVector v, BVFloat4 & r)
		{
			VectorToFloat4Ptr(v, &r.x);
		}

		inline float VectorGetX(CBVVector v)
		{
			return v.x;
		}

		inline float VectorGetY(CBVVector v)
		{
			return v.y;
		}

		inline float VectorGetZ(CBVVector v)
		{
			return v.z;
		}

		inline float VectorGetW(CBVVector v)
		{
			return v.w;
		}

		inline BVVector VectorSet(const float x, const float y, const float z, const float w)
		{
			return BVVector(x, y, z, w);
		}

		// ======================
		// Basic Operations
		// ======================

		inline BVVector VectorAdd(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
		}

		inline BVVector VectorSub(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
		}

		inline BVVector VectorMul(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
		}

		inline BVVector VectorDiv(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
		}

		inline BVVector VectorMul(CBVVector v, const float val)
		{
			return BVVector(v.x * val, v.y * val, v.z * val, v.w * val);
		}

		inline BVVector VectorDiv(CBVVector v, const float val)
		{
			return BVVector(v.x / val, v.y / val, v.z / val, v.w / val);
		}

		inline BVVector VectorMul(CBVVector v, CRBVMatrix m)
		{
			return BVVector(v.x * m.r[0].x + v.y * m.r[1].x + v.z * m.r[2].x + v.w * m.r[3].x,
				v.x * m.r[0].y + v.y * m.r[1].y + v.z * m.r[2].y + v.w * m.r[3].y,
				v.x * m.r[0].z + v.y * m.r[1].z + v.z * m.r[2].z + v.w * m.r[3].z,
				v.x * m.r[0].w + v.y * m.r[1].w + v.z * m.r[2].w + v.w * m.r[3].w
				);
		}


		inline bool VectorIsZero(CBVVector v, const float epsilon)
		{
			return (v.x <= epsilon && v.x >= -epsilon
				&& v.y <= epsilon && v.y >= -epsilon
				&& v.z <= epsilon && v.z >= -epsilon
				&& v.w <= epsilon && v.w >= -epsilon);
		}

		inline bool VectorIsEqual(CBVVector v1, CBVVector v2, const float epsilon)
		{
			return VectorIsZero(VectorSub(v1, v2), epsilon);
		}

		inline BVVector VectorNegate(CBVVector v)
		{
			return BVVector(-v.x, -v.y, -v.z, -v.w);
		}

		inline BVVector VectorZero()
		{
			return BVVector();
		}

		inline BVVector VectorReplicate(const float s)
		{
			return BVVector(s, s, s, s);
		}

		inline BVVector VectorReplicateX(CBVVector v)
		{
			return BVVector(v.x, v.x, v.x, v.x);
		}

		inline BVVector VectorReplicateY(CBVVector v)
		{
			return BVVector(v.y, v.y, v.y, v.y);
		}

		inline BVVector VectorReplicateZ(CBVVector v)
		{
			return BVVector(v.z, v.z, v.z, v.z);
		}

		inline BVVector VectorReplicateW(CBVVector v)
		{
			return BVVector(v.w, v.w, v.w, v.w);
		}

		inline BVVector VectorMax(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x > v2.x ? v1.x : v2.x,
				v1.y > v2.y ? v1.y : v2.y,
				v1.z > v2.z ? v1.z : v2.z,
				v1.w > v2.w ? v1.w : v2.w);
		}

		inline BVVector VectorMin(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.x < v2.x ? v1.x : v2.x,
				v1.y < v2.y ? v1.y : v2.y,
				v1.z < v2.z ? v1.z : v2.z,
				v1.w < v2.w ? v1.w : v2.w);
		}


		// ======================
		// Operators
		// ======================

		inline BVVector operator + (CBVVector v)
		{
			return v;
		}

		inline BVVector operator - (CBVVector v)
		{
			return VectorNegate(v);
		}

		inline BVVector operator + (CBVVector v1, CBVVector v2)
		{
			return VectorAdd(v1, v2);
		}

		inline BVVector & operator += (RBVVector v1, CBVVector v2)
		{
			v1 = VectorAdd(v1, v2);
			return v1;
		}

		inline BVVector operator - (CBVVector v1, CBVVector v2)
		{
			return VectorSub(v1, v2);
		}

		inline BVVector & operator -= (RBVVector v1, CBVVector v2)
		{
			v1 = VectorSub(v1, v2);
			return v1;
		}

		inline BVVector operator * (CBVVector v1, CBVVector v2)
		{
			return VectorMul(v1, v2);
		}

		inline BVVector & operator *= (RBVVector v1, CBVVector v2)
		{
			v1 = VectorMul(v1, v2);
			return v1;
		}

		inline BVVector operator * (CBVVector v, const float s)
		{
			return VectorMul(v, s);
		}

		inline BVVector operator * (const float s, CBVVector v)
		{
			return VectorMul(v, s);
		}

		inline BVVector & operator *= (RBVVector v, const float s)
		{
			v = VectorMul(v, s);
			return v;
		}

		inline BVVector operator * (CBVVector v, CRBVMatrix m)
		{
			return VectorMul(v, m);
		}

		inline BVVector & operator *= (RBVVector v, CRBVMatrix m)
		{
			v = VectorMul(v, m);
			return v;
		}

		inline BVVector operator / (CBVVector v1, CBVVector v2)
		{
			return VectorDiv(v1, v2);
		}

		inline BVVector & operator /= (RBVVector v1, CBVVector v2)
		{
			v1 = VectorDiv(v1, v2);
			return v1;
		}

		inline BVVector operator / (CBVVector v, const float s)
		{
			return VectorDiv(v, s);
		}

		inline BVVector & operator /= (RBVVector v, const float s)
		{
			v = VectorDiv(v, s);
			return v;
		}

		// ======================
		// Specialized Operations
		// ======================

		inline float VectorDot(CBVVector v1, CBVVector v2)
		{
			return VectorDotV(v1, v2).x;
		}

		inline BVVector VectorDotV(CBVVector v1, CBVVector v2)
		{
			float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
			return BVVector(dot, dot, dot, dot);
		}

		inline BVVector VectorCross(CBVVector v1, CBVVector v2)
		{
			return BVVector(v1.y * v2.z - v2.y * v1.z,
				v1.z * v2.x - v2.z * v1.x,
				v1.x * v2.y - v2.x * v1.y);
		}

		inline float VectorLengthSqr(CBVVector v)
		{
			return VectorLengthSqrV(v).x;
		}

		inline BVVector VectorLengthSqrV(CBVVector v)
		{
			return VectorDotV(v, v);
		}

		inline float VectorLength(CBVVector v)
		{
			return VectorLengthV(v).x;
		}

		inline BVVector VectorLengthV(CBVVector v)
		{
			BVVector r = VectorLengthSqrV(v);
			r.x = sqrtf(r.x);
			r.y = r.x;
			r.z = r.x;
			r.w = r.x;
			return r;
		}

		inline BVVector VectorNormalize(CBVVector v)
		{
			return VectorMul(v, 1.0f / VectorLengthSqr(v));
		}

		inline BVVector VectorProject(CBVVector v, CBVVector p)
		{
			// To project on Vector onto another
			// get the dot product between the Vectors
			// and multiply the value by the unit Vector
			// of the Vector to be projected on
			// Projection = (V.P)P
			BVVector vDotP = VectorDotV(v, p);
			BVVector pN = VectorNormalize(p);
			return VectorMul(vDotP, pN);
		}

		inline BVVector VectorLerp(CBVVector v1, CBVVector v2, float t)
		{
			// Lerp = V0 + t * (V1 - V0)
			BVVector vT = VectorReplicate(t);
			BVVector v2MinusV1 = VectorSub(v2, v1);

			return VectorAdd(v1, VectorMul(vT, v2MinusV1));
		}

		inline BVVector VectorReflect(CBVVector i, CBVVector n)
		{
			// Reflection = I - 2(I.N)N
			BVVector twoTimesProj = VectorProject(i, n);
			twoTimesProj = VectorMul(twoTimesProj, 2.0f);

			return VectorSub(i, twoTimesProj);
		}

		inline BVVector VectorTransformDir(CBVVector v, CRBVMatrix m)
		{
			return BVVector(
				v.x * m.r[0].x + v.y * m.r[1].x + v.z * m.r[2].x,
				v.x * m.r[0].y + v.y * m.r[1].y + v.z * m.r[2].y,
				v.x * m.r[0].z + v.y * m.r[1].z + v.z * m.r[2].z,
				0.0f
			);
		}

		inline BVVector VectorTransformPoint(CBVVector v, CRBVMatrix m)
		{
			return BVVector(
				v.x * m.r[0].x + v.y * m.r[1].x + v.z * m.r[2].x + m.r[3].x,
				v.x * m.r[0].y + v.y * m.r[1].y + v.z * m.r[2].y + m.r[3].y,
				v.x * m.r[0].z + v.y * m.r[1].z + v.z * m.r[2].z + m.r[3].z,
				1.0f
			);
		}
	}
}


#endif