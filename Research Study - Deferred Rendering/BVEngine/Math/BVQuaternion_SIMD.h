#ifndef BVQUATERNION_SIMD_H
#define BVQUATERNION_SIMD_H

#include "BVVector_SIMD.h"

namespace BVMath
{
	namespace SIMD
	{
		// ==================================
		// Quaternion operations
		// ==================================

		// ======================
		// Declarations
		// ======================

		// ======================
		// Specialized Operations
		// ======================

		BVQuat QuaternionMultiply(CBVQuat q1, CBVQuat q2);
		BVQuat QuaternionMultiplyKeenan(CBVQuat q1, CBVQuat q2);

		BVQuat QuaternionConjugate(CBVQuat q);
		BVQuat QuaternionInverse(CBVQuat q);

		BVQuat QuaternionNormalize(CBVQuat q);

		float QuaternionDot(CBVQuat q1, CBVQuat q2);
		BVQuat QuaternionDotV(CBVQuat q1, CBVQuat q2);

		float QuaternionLengthSqr(CBVQuat q);
		BVQuat QuaternionLengthSqrV(CBVQuat q);

		float QuaternionLength(CBVQuat q);
		BVQuat QuaternionLengthV(CBVQuat q);

		BVQuat QuaternionRotationAxis(CBVVector v, float angle);
		BVVector QuaternionQVQC(CBVQuat q, CBVVector v);
		BVVector QuaternionQCVQ(CBVQuat q, CBVVector v);

		BVVector QuaternionQVQCKeenan(CBVQuat q, CBVVector v);
		BVVector QuaternionQCVQKeenan(CBVQuat q, CBVVector v);

		BVMatrix QuaternionToMatrix(CBVQuat q);
		BVQuat QuaternionFromMatrix(CRBVMatrix m);

		BVQuat QuaternionSlerp(CBVQuat q1, CBVQuat q2, const float t, const float epsilon = BV_EPSILON);

		float QuaternionAngle(CBVQuat q);

		// =================
		// Definitions
		// =================

		// ======================
		// Specialized Operations
		// ======================

		inline BVQuat QuaternionMultiply(CBVQuat q1, CBVQuat q2)
		{
			// Quaternion Multiplication
			// [q1s * q2v + q2s * q1v + q1 x q2] [q1s * q2s - q1.q2]
			// Expanding the equation
			// x => q1.w * q2.x + q2.w * q1.x + q1.y * q2.z - q1.z * q2.y
			// y => q1.w * q2.y + q2.w * q1.y + q1.z * q2.x - q1.x * q2.z
			// z => q1.w * q2.z + q2.w * q1.z + q1.x * q2.y - q1.y * q2.x
			// w => q1.w * q2.w - q2.x * q1.x - q1.y * q2.y - q1.z * q2.z

			// v0 = q1.w, q1.w, q1.w, q1.w
			BVVector v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 3, 3, 3));

			// r = q1.w * q2.x, q1.w * q2.y, q1.w * q2.z, q1.w * q2.w
			BVVector r = _mm_mul_ps(v0, q2);

			// v1 = q2.w, q2.w, q2.w, q2.x
			BVVector v1 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 3, 3, 3));
			// v2 = q1.x, q1.y, q1.z, q1.x
			BVVector v2 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 2, 1, 0));

			// c0 = 1.0, 1.0, 1.0, -1.0
			BVVector c0 = _mm_set_ps(-1.0f, 1.0f, 1.0f, 1.0f);

			// v1 = q2.w * q1.x, q2.w * q1.y, q2.w * q1.z, q2.x * q1.x
			v1 = _mm_mul_ps(v1, v2);
			// v1 = q2.w * q1.x, q2.w * q1.y, q2.w * q1.z, -q2.x * q1.x
			v1 = _mm_mul_ps(v1, c0);

			// Perform first addition
			r = _mm_add_ps(r, v1);

			// v1 = q1.y, q1.z, q1.x, q1.y
			v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 0, 2, 1));
			// v2 = q2.z, q2.x, q2.y, q2.y
			v2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 1, 0, 2));

			// v1 = q1.y * q2. z, q1.z * q2.x, q1.x * q2.y, q1.y * q2.y
			v1 = _mm_mul_ps(v1, v2);
			// v1 = q1.y * q2. z, q1.z * q2.x, q1.x * q2.y, -q1.y * q2.y
			v1 = _mm_mul_ps(v1, c0);

			// Perform second addition
			r = _mm_add_ps(r, v1);

			// v1 = q1.z, q1.x, q1.y, q1.z
			v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 1, 0, 2));
			// v2 = q2.y, q2.z, q2.x, q2.z
			v2 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 0, 2, 1));

			// v1 = q1.z * q2.y, q1.x * q2.z, q1.y * q2.x, q1.z * q2.z
			v1 = _mm_mul_ps(v1, v2);

			// Perform a subtraction
			r = _mm_sub_ps(r, v1);

			return r;
		}

		inline BVQuat QuaternionMultiplyKeenan(CBVQuat q1, CBVQuat q2)
		{
			// Quaternion Multiplication
			// [q1s * q2v + q2s * q1v + q2 x q1] [q1s * q2s - q1.q2]
			// Expanding the equation
			// x => q1.w * q2.x + q2.w * q1.x + q2.y * q1.z - q2.z * q1.y
			// y => q1.w * q2.y + q2.w * q1.y + q2.z * q1.x - q2.x * q1.z
			// z => q1.w * q2.z + q2.w * q1.z + q2.x * q1.y - q2.y * q1.x
			// w => q1.w * q2.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z

			// v0 = q1.w, q1.w, q1.w, q1.w
			BVVector v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 3, 3, 3));

			// r = q1.w * q2.x, q1.w * q2.y, q1.w * q2.z, q1.w * q2.w
			BVVector r = _mm_mul_ps(v0, q2);

			// v1 = q2.w, q2.w, q2.w, q2.x
			BVVector v1 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(0, 3, 3, 3));
			// v2 = q1.x, q1.y, q1.z, q1.x
			BVVector v2 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 2, 1, 0));

			// c0 = 1.0, 1.0, 1.0, -1.0
			BVVector c0 = _mm_set_ps(-1.0f, 1.0f, 1.0f, 1.0f);

			// v1 = q2.w * q1.x, q2.w * q1.y, q2.w * q1.z, q2.x * q1.x
			v1 = _mm_mul_ps(v1, v2);
			// v1 = q2.w * q1.x, q2.w * q1.y, q2.w * q1.z, -q2.x * q1.x
			v1 = _mm_mul_ps(v1, c0);

			// Perform first addition
			r = _mm_add_ps(r, v1);

			// v1 = q2.y, q2.z, q2.x, q2.y
			v1 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 0, 2, 1));
			// v2 = q1.z, q1.x, q1.y, q1.y
			v2 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 1, 0, 2));

			// v1 = q2.y * q1. z, q2.z * q1.x, q2.x * q1.y, q2.y * q1.y
			v1 = _mm_mul_ps(v1, v2);
			// v1 = q2.y * q1. z, q2.z * q1.x, q2.x * q1.y, -q2.y * q1.y
			v1 = _mm_mul_ps(v1, c0);

			// Perform second addition
			r = _mm_add_ps(r, v1);

			// v1 = q2.z, q2.x, q2.y, q2.z
			v1 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 1, 0, 2));
			// v2 = q1.y, q1.z, q1.x, q1.z
			v2 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 0, 2, 1));

			// v1 = q2.z * q1.y, q2.x * q1.z, q2.y * q1.x, q2.z * q1.z
			v1 = _mm_mul_ps(v1, v2);

			// Perform a subtraction
			r = _mm_sub_ps(r, v1);

			return r;
		}

		inline BVQuat QuaternionConjugate(CBVQuat q)
		{
			BVQuat conj = _mm_set_ps(1.0f, -1.0f, -1.0f, -1.0f);

			return _mm_mul_ps(q, conj);
		}

		inline BVQuat QuaternionInverse(CBVQuat q)
		{
			BVQuat conj = QuaternionConjugate(q);
			BVQuat oneOverLengthSqr = _mm_rcp_ps(QuaternionLengthSqrV(q));

			return _mm_mul_ps(conj, oneOverLengthSqr);
		}

		inline BVQuat QuaternionNormalize(CBVQuat q)
		{
			return _mm_mul_ps(q, _mm_rsqrt_ps(QuaternionLengthSqrV(q)));
		}

		inline float QuaternionDot(CBVQuat q1, CBVQuat q2)
		{
			// _mm_cvtss_f32 gets the value of the lowest float, in this case, X
			// _mm_store_ss could also be used, but I think it might be slower
			return _mm_cvtss_f32(QuaternionDotV(q1, q2));
		}

		inline BVQuat QuaternionDotV(CBVQuat q1, CBVQuat q2)
		{
			// _mm_dp_ps will use a 8 bit mask where:
			// bits 7, 6, 5 and 4 describe which elements to multiply
			// bits 3, 2, 1 and 0 describe where elements will be stored

			// For this implementation, 1111 1111 will be used, so x, y, z and w will get multiplied
			// and the value is stored across all elements
			return _mm_dp_ps(q1, q2, 0xFF);
		}

		inline float QuaternionLengthSqr(CBVQuat q)
		{
			return _mm_cvtss_f32(QuaternionLengthSqrV(q));
		}

		inline BVQuat QuaternionLengthSqrV(CBVQuat q)
		{
			return QuaternionDotV(q, q);
		}

		inline float QuaternionLength(CBVQuat v)
		{
			return _mm_cvtss_f32(QuaternionLengthV(v));
		}

		inline BVQuat QuaternionLengthV(CBVQuat v)
		{
			return _mm_sqrt_ps(QuaternionLengthSqrV(v));
		}

		inline BVQuat QuaternionRotationAxis(CBVVector v, float angle)
		{
			float halfAngle = angle * 0.5f;
			float sinCos[] = { sinf(halfAngle), cosf(halfAngle) };
			BVVector q = VectorSet(sinCos[0], sinCos[0], sinCos[0], sinCos[1]);
			BVVector n = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			n = VectorAdd(n, v);

			return VectorMul(q, n);
		}

		inline BVVector QuaternionQVQC(CBVQuat q, CBVVector v)
		{
			BVVector v0 = VectorReplicateW(q);
			BVVector v1 = VectorMul(v0, v0);
			v0 = VectorAdd(v0, v0);

			BVVector v2 = VectorDotV(q, q);
			v2 = VectorSub(v1, v2);
			v2 = VectorMul(v2, v);

			v1 = VectorCross(q, v);
			v1 = VectorMul(v0, v1);

			v0 = VectorDotV(q, v);
			v0 = VectorAdd(v0, v0);
			v0 = VectorMul(v0, q);

			return VectorAdd(v0, VectorAdd(v1, v2));
		}

		inline BVVector QuaternionQCVQ(CBVQuat q, CBVVector v)
		{
			BVVector v0 = VectorReplicateW(q);
			BVVector v1 = VectorMul(v0, v0);
			v0 = VectorAdd(v0, v0);

			BVVector v2 = VectorDotV(q, q);
			v2 = VectorSub(v1, v2);
			v2 = VectorMul(v2, v);

			v1 = VectorCross(v, q);
			v1 = VectorMul(v0, v1);

			v0 = VectorDotV(q, v);
			v0 = VectorAdd(v0, v0);
			v0 = VectorMul(v0, q);

			return VectorAdd(v0, VectorAdd(v1, v2));
		}

		inline BVVector QuaternionQVQCKeenan(CBVQuat q, CBVVector v)
		{
			return QuaternionQCVQ(q, v);
		}

		inline BVVector QuaternionQCVQKeenan(CBVQuat q, CBVVector v)
		{
			return QuaternionQVQC(q, v);
		}

		inline BVMatrix QuaternionToMatrix(CBVQuat q)
		{
			// v0 = 2 * q.x, 2 * q.y, 2 * q.z, 2 * q.w
			BVVector v0 = _mm_add_ps(q, q);

			// v1 = 2 * q.x, 2 * q.x, 2 * q.x, 2 * q.y
			BVVector v1 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 0, 0, 0));
			// v3 = q.y, q.z, q.y, q.z
			BVVector v3 = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 1, 2, 1));
			// v1 = 2 * q.x * q.y, 2 * q.x * q.z, 2 * q.x * q.y, 2 * q.y * q.z
			v1 = _mm_mul_ps(v1, v3);

			// v2 = 2 * q.w, 2 * q.w, 2 * q.w, 2 * q.w
			BVVector v2 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 3));
			// v3 = q.z, q.y, q.z, q.x
			v3 = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 2, 1, 2));
			// v2 = 2 * q.w * q.z, 2 * q.w * q.y, 2 * q.w * q.z, 2 * q.w * q.x
			v2 = _mm_mul_ps(v2, v3);


			// v3 = 2 * q.x * q.y + 2 * q.w * q.z
			//      2 * q.x * q.z + 2 * q.w * q.y
			//      2 * q.x * q.y + 2 * q.w * q.z
			//      2 * q.y * q.z + 2 * q.w * q.x
			v3 = _mm_add_ps(v1, v2);

			// v2 = 2 * q.x * q.y - 2 * q.w * q.z
			//      2 * q.x * q.z - 2 * q.w * q.y
			//      2 * q.x * q.y - 2 * q.w * q.z
			//      2 * q.y * q.z - 2 * q.w * q.x
			v2 = _mm_sub_ps(v1, v2);

			// v1 = 2 * q.x * q.y + 2 * q.w * q.z
			//      2 * q.y * q.z + 2 * q.w * q.x
			//      2 * q.x * q.z - 2 * q.w * q.y
			//      2 * q.x * q.y - 2 * q.w * q.z
			v1 = _mm_shuffle_ps(v3, v2, _MM_SHUFFLE(0, 1, 3, 0));

			// v2 = 2 * q.x * q.z + 2 * q.w * q.y
			//      2 * q.x * q.z + 2 * q.w * q.y
			//      2 * q.y * q.z - 2 * q.w * q.x
			//      2 * q.y * q.z - 2 * q.w * q.x
			v2 = _mm_shuffle_ps(v3, v2, _MM_SHUFFLE(3, 3, 1, 1));

			// c0 = 1, 1, 1, 0
			BVVector c0 = _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f);

			// v0 = 2 * q.x ^ 2, 2 * q.y ^ 2, 2 * q.z ^ 2, 2 * q.w ^ 2
			v0 = _mm_mul_ps(v0, q);
			// v0 = 2 * q.x ^ 2, 2 * q.y ^ 2, 2 * q.z ^ 2, 0
			v0 = _mm_mul_ps(c0, v0);
			// v0 = 2 * q.x ^ 2 + 2 * q.y ^ 2, 2 * q.y ^ 2 + 2 * q.z ^ 2, 2 * q.z ^ 2 + 2 * q.x ^ 2, 0
			v0 = _mm_add_ps(v0, _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 0, 2, 1)));
			// v0 = 1 - 2 * q.x ^ 2 - 2 * q.y ^ 2, 1 - 2 * q.y ^ 2 - 2 * q.z ^ 2, 1 - 2 * q.z ^ 2 - 2 * q.x ^ 2, 0
			v0 = _mm_sub_ps(c0, v0);

			// v3 = 1 - 2 * q.y ^ 2 - 2 * q.z ^ 2, 0, 2 * q.x * q.y + 2 * q.w * q.z, 2 * q.x * q.z - 2 * q.w * q.y
			v3 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(2, 0, 3, 1));
			// v3 = 1 - 2 * q.y ^ 2 - 2 * q.z ^ 2, 2 * q.x * q.y + 2 * q.w * q.z, 2 * q.x * q.z - 2 * q.w * q.y, 0
			v3 = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(1, 3, 2, 0));

			BVMatrix r;
			r.r[0] = v3;

			// v3 = 1 - 2 * q.z ^ 2 - 2 * q.x ^ 2, 0, 2 * q.x * q.y - 2 * q.w * q.z, 2 * q.y * q.z + 2 * q.w * q.x
			v3 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(1, 3, 3, 2));
			// v3 = 2 * q.x * q.y - 2 * q.w * q.z, 1 - 2 * q.z ^ 2 - 2 * q.x ^ 2, 2 * q.y * q.z + 2 * q.w * q.x, 0
			v3 = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(1, 3, 0, 2));

			r.r[1] = v3;

			// v3 = 1 - 2 * q.x ^ 2 - 2 * q.y ^ 2, 0, 2 * q.x * q.z + 2 * q.w * q.y, 2 * q.y * q.z - 2 * q.w * q.x
			v3 = _mm_shuffle_ps(v0, v2, _MM_SHUFFLE(2, 0, 3, 0));
			// v3 = 2 * q.x * q.z + 2 * q.w * q.y, 2 * q.y * q.z - 2 * q.w * q.x, 1 - 2 * q.x ^ 2 - 2 * q.y ^ 2, 0
			v3 = _mm_shuffle_ps(v3, v3, _MM_SHUFFLE(1, 0, 3, 2));

			r.r[2] = v3;

			// c0 = 0, 0, 0, 1
			c0 = _mm_shuffle_ps(c0, c0, _MM_SHUFFLE(0, 3, 3, 3));

			r.r[3] = c0;

			return r;
		}

		inline BVQuat QuaternionFromMatrix(CRBVMatrix m)
		{
			BVVector v0;

			// z0 = 0, 0, 0, 0
			BVVector z0 = _mm_setzero_ps();

			BVVector if0, if1, if2, if3;
			{
				// m00 = m00, m00, m00, m00
				BVVector m00 = _mm_shuffle_ps(m.r[0], m.r[0], _MM_SHUFFLE(0, 0, 0, 0));
				// m11 = m11, m11, m11, m11
				BVVector m11 = _mm_shuffle_ps(m.r[1], m.r[1], _MM_SHUFFLE(1, 1, 1, 1));
				// m22 = m22, m22, m22, m22
				BVVector m22 = _mm_shuffle_ps(m.r[2], m.r[2], _MM_SHUFFLE(2, 2, 2, 2));

				// v0 = m00 + m11, m00 + m11, m00 + m11, m00 + m11
				v0 = _mm_add_ps(m00, m11);
				// v0 = m00 + m011 + m22, m00 + m011 + m22, m00 + m011 + m22, m00 + m011 + m22
				v0 = _mm_add_ps(v0, m22);

				// Check all 4 conditions

				// if0 = v0 > z0 ? 0xffffffff : 0
				if0 = _mm_cmpgt_ps(v0, z0);
				// if1 = m00 > m11 ? 0xffffffff : 0
				if1 = _mm_cmpgt_ps(m00, m11);
				// if11 = m00 > m22 ? 0xffffffff : 0
				BVVector if11 = _mm_cmpgt_ps(m00, m22);
				// if1 = if1  > 0 ? 0xffffffff : 0
				if1 = _mm_cmpgt_ps(if1, z0);
				// if11 = if11  > 0 ? 0xffffffff : 0
				if11 = _mm_cmpgt_ps(if11, z0);
				// if1 = if1 && if11
				if1 = _mm_and_ps(if1, if11);

				// if2 = m11 > m22 ? 0xffffffff : 0
				if2 = _mm_cmpgt_ps(m11, m22);

				// if3 = if0 | if1
				if3 = _mm_or_ps(if0, if1);
				// if3 = if0 | if1 | if2
				if3 = _mm_or_ps(if3, if2);
				// if3 = (if0 | if1 | if2) == 0 ? 0xffffffff : 0
				if3 = _mm_cmpeq_ps(if3, z0);
			}


			// m0 = m00, m00, m11, m11
			BVVector m0 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(1, 1, 0, 0));
			// m1 = 1, 1, -1, -1
			BVVector m1 = _mm_set_ps(-1.0f, -1.0f, 1.0f, 1.0f);
			// m2 = m22, m22, 1, 1
			BVVector m2 = _mm_shuffle_ps(m.r[2], m1, _MM_SHUFFLE(0, 0, 2, 2));

			// m0 = m00, m11, m22, 1
			m0 = _mm_shuffle_ps(m0, m2, _MM_SHUFFLE(2, 0, 2, 0));
			// m0 = 1, m00, m11, m22
			m0 = _mm_shuffle_ps(m0, m0, _MM_SHUFFLE(2, 1, 0, 3));


			// Control signs

			// c0 = 1, 1, 1, 1
			BVVector c0 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(0, 0, 0, 0));
			// c1 = 1, 1, -1, -1
			BVVector c1 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(2, 2, 0, 0));
			// c2 = 1, -1, 1, -1
			BVVector c2 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(2, 0, 2, 0));
			// c3 = 1, -1, -1, 1
			BVVector c3 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(0, 2, 2, 0));

			// m2 = 0.25, 0, 0, 0
			m2 = _mm_set_ss(0.25f);

			// First block of the component-wise addition

			// s0 = m02, m02, m10, m10
			BVVector s0 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(0, 0, 2, 2));
			// s0 = m23, m21, m02, m10
			s0 = _mm_shuffle_ps(m.r[2], s0, _MM_SHUFFLE(2, 0, 1, 3));
			// s0 = 0.25, m21, m02, m10
			s0 = _mm_move_ss(s0, m2);

			// Second block of the component-wise addition

			// s1 = m12, m12, m20, m20
			BVVector s1 = _mm_shuffle_ps(m.r[1], m.r[2], _MM_SHUFFLE(0, 0, 2, 2));
			// s1 = 0, m01, m12, m20
			s1 = _mm_shuffle_ps(m.r[0], s1, _MM_SHUFFLE(2, 0, 1, 3));
			// s1 = 0, m12, m20, m01
			s1 = _mm_shuffle_ps(s1, s1, _MM_SHUFFLE(1, 3, 2, 0));

			// Check every condition
			c0 = _mm_and_ps(c0, if0);
			c1 = _mm_and_ps(c1, if1);
			c2 = _mm_and_ps(c2, if2);
			c3 = _mm_and_ps(c3, if3);

			// Then concantenate them all
			c0 = _mm_or_ps(c0, c1);
			c0 = _mm_or_ps(c0, c2);
			c0 = _mm_or_ps(c0, c3);

			// Change the signs of the values that will be summed
			m0 = _mm_mul_ps(m0, c0);
			// m0 = m0.x + m0.y, m0.z + m0.w, m0.x + m0.y, m0.z + m0.w
			m0 = _mm_hadd_ps(m0, m0);
			// m0 = m0.x + m0.y + m0.z + m0.w, m0.x + m0.y + m0.z + m0.w, m0.x + m0.y + m0.z + m0.w, m0.x + m0.y + m0.z + m0.w
			m0 = _mm_hadd_ps(m0, m0);
			m0 = _mm_rsqrt_ps(m0);
			c0 = _mm_set1_ps(0.5f);
			m0 = _mm_mul_ps(m0, c0);
			m0 = _mm_rcp_ss(m0);


			// Control signs for the formula

			// c0 = -1, -1, -1, 1
			c0 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(0, 2, 2, 2));
			// c1 = 1, 1, 1, -1
			c1 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(2, 0, 0, 0));
			// c2 = 1, 1, 1, -1
			c2 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(2, 0, 0, 0));
			// c3 = 1, 1, 1, -1
			c3 = _mm_shuffle_ps(m1, m1, _MM_SHUFFLE(2, 0, 0, 0));

			// Check every condition
			c0 = _mm_and_ps(c0, if0);
			c1 = _mm_and_ps(c1, if1);
			c2 = _mm_and_ps(c2, if2);
			c3 = _mm_and_ps(c3, if3);

			// Then concantenate them all
			c0 = _mm_or_ps(c0, c1);
			c0 = _mm_or_ps(c0, c2);
			c0 = _mm_or_ps(c0, c3);

			s1 = _mm_mul_ps(s1, c0);
			s0 = _mm_add_ps(s0, s1);
			s0 = _mm_mul_ps(s0, m0);

			{
				BVVector shuffle0 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(0, 3, 2, 1));
				BVVector shuffle1 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(1, 2, 3, 0));
				BVVector shuffle2 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(2, 1, 0, 3));
				BVVector shuffle3 = _mm_shuffle_ps(s0, s0, _MM_SHUFFLE(3, 0, 1, 2));

				shuffle0 = _mm_and_ps(shuffle0, if0);
				shuffle1 = _mm_and_ps(shuffle1, if1);
				shuffle2 = _mm_and_ps(shuffle2, if2);
				shuffle3 = _mm_and_ps(shuffle3, if3);

				// Then concantenate them all
				shuffle0 = _mm_or_ps(shuffle0, shuffle1);
				shuffle0 = _mm_or_ps(shuffle0, shuffle2);
				shuffle0 = _mm_or_ps(shuffle0, shuffle3);

				s0 = shuffle0;
			}

			return QuaternionNormalize(s0);
		}

		inline BVQuat QuaternionSlerp(CBVQuat q1, CBVQuat q2, const float t, const float epsilon)
		{
			BVVector c0 = QuaternionDotV(q1, q2);
			BVVector v0 = VectorNegate(c0);
			BVQuat q = VectorNegate(q2);

			BVVector v1 = VectorZero();
			BVVector c1 = _mm_cmplt_ps(c0, v1);

			v0 = _mm_and_ps(v0, c1);
			c0 = _mm_andnot_ps(c1, c0);
			c0 = _mm_or_ps(c0, v0);

			q = _mm_and_ps(q, c1);
			q = _mm_or_ps(q, _mm_andnot_ps(c1, q2));

			v0 = VectorReplicate(epsilon);

			BVVector vAngle = VectorReplicate(acosf(VectorGetX(c0)));
			c1 = _mm_cmpgt_ps(vAngle, v0);

			c0 = VectorReplicate(1.0f);
			BVVector lerp2 = VectorReplicate(t);
			BVVector lerp1 = _mm_sub_ps(lerp1, lerp2);

			BVVector oneOverSinAngle = VectorReplicate(sinf(VectorGetX(vAngle)));
			oneOverSinAngle = _mm_rcp_ps(oneOverSinAngle);
			v1 = _mm_mul_ps(lerp1, vAngle);
			v1 = VectorReplicate(sinf(VectorGetX(v1)));
			v1 = _mm_mul_ps(v1, oneOverSinAngle);

			BVVector v2 = _mm_mul_ps(lerp2, vAngle);
			v2 = VectorReplicate(sinf(VectorGetX(v2)));
			v2 = _mm_mul_ps(v2, oneOverSinAngle);

			v1 = _mm_and_ps(c1, v1);
			v2 = _mm_and_ps(c1, v2);

			lerp1 = _mm_andnot_ps(c1, lerp1);
			lerp2 = _mm_andnot_ps(c1, lerp2);

			v1 = _mm_or_ps(v1, lerp1);
			v2 = _mm_or_ps(v2, lerp2);

			v1 = _mm_mul_ps(v1, q1);
			v2 = _mm_mul_ps(v2, q);

			return _mm_add_ps(v1, v2);
		}

		inline float QuaternionAngle(CBVQuat q)
		{
			return 2.0f * acosf(VectorGetW(q));
		}
	}
}

#endif