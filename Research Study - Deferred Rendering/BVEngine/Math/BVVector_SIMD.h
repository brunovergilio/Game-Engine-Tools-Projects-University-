#ifndef BVVECTOR_SIMD_H
#define BVVECTOR_SIMD_H

#include "BVMath.h"

namespace BVMath
{
	namespace SIMD
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
			return _mm_load_ss(p);
		}

		inline BVVector VectorFromFloat2Ptr(const float * const p)
		{
			BVVector x = _mm_load_ss(p);
			BVVector y = _mm_load_ss(p + 1);
			BVVector xy = _mm_movelh_ps(x, y);
			return _mm_shuffle_ps(xy, xy, _MM_SHUFFLE(3, 1, 2, 0));
		}

		inline BVVector VectorFromFloat3Ptr(const float * const p)
		{
			BVVector x = _mm_load_ss(p);
			BVVector y = _mm_load_ss(p + 1);
			BVVector z = _mm_load_ss(p + 2);
			BVVector xy = _mm_movelh_ps(x, y);
			return _mm_shuffle_ps(xy, z, _MM_SHUFFLE(2, 0, 2, 0));
		}

		inline BVVector VectorFromFloat4Ptr(const float * const p)
		{
			return _mm_loadu_ps(p);
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
			_mm_store_ss(p, v);
		}

		inline void VectorToFloat2Ptr(CBVVector v, float * const p)
		{
			_mm_store_ss(p, v);
			_mm_store_ss(p + 1, _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
		}

		inline void VectorToFloat3Ptr(CBVVector v, float * const p)
		{
			_mm_store_ss(p, v);
			_mm_store_ss(p + 1, _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
			_mm_store_ss(p + 2, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)));
		}

		inline void VectorToFloat4Ptr(CBVVector v, float * const p)
		{
			_mm_storeu_ps(p, v);
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
			return _mm_cvtss_f32(v);
		}

		inline float VectorGetY(CBVVector v)
		{
			BVVector y = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			return _mm_cvtss_f32(y);
		}

		inline float VectorGetZ(CBVVector v)
		{
			BVVector z = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			return _mm_cvtss_f32(z);
		}

		inline float VectorGetW(CBVVector v)
		{
			BVVector w = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
			return _mm_cvtss_f32(w);
		}

		inline BVVector VectorSet(const float x, const float y, const float z, const float w)
		{
			return _mm_set_ps(w, z, y, x);
		}

		// ======================
		// Basic Operations
		// ======================

		inline BVVector VectorAdd(CBVVector v1, CBVVector v2)
		{
			return _mm_add_ps(v1, v2);
		}

		inline BVVector VectorSub(CBVVector v1, CBVVector v2)
		{
			return _mm_sub_ps(v1, v2);
		}

		inline BVVector VectorMul(CBVVector v1, CBVVector v2)
		{
			return _mm_mul_ps(v1, v2);
		}

		inline BVVector VectorDiv(CBVVector v1, CBVVector v2)
		{
			return _mm_div_ps(v1, v2);
		}

		inline BVVector VectorMul(CBVVector v, const float val)
		{
			return _mm_mul_ps(v, _mm_set_ps1(val));
		}

		inline BVVector VectorDiv(CBVVector v, const float val)
		{
			return _mm_div_ps(v, _mm_set_ps1(val));
		}

		inline BVVector VectorMul(CBVVector v, CRBVMatrix m)
		{
			BVVector tmp0 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
			tmp0 = _mm_mul_ps(tmp0, m.r[0]);

			BVVector tmp1 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
			tmp1 = _mm_mul_ps(tmp1, m.r[1]);

			BVVector tmp2 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
			tmp2 = _mm_mul_ps(tmp2, m.r[2]);

			BVVector tmp3 = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
			tmp3 = _mm_mul_ps(tmp3, m.r[3]);

			BVVector r = _mm_add_ps(tmp0, tmp1);
			r = _mm_add_ps(r, _mm_add_ps(tmp2, tmp3));

			return r;
		}


		inline bool VectorIsZero(CBVVector v, const float epsilon)
		{
			BVVector e = _mm_set1_ps(epsilon);
			BVVector z = _mm_setzero_ps();
			z = _mm_sub_ps(z, v);
			z = _mm_max_ps(z, v);
			// cmple (compare less or equal) will check the condition for every element
			// the result will be either 0xffffffff if true or 0 if false (per element)
			z = _mm_cmple_ps(z, e);

			// movemask will get the most significant bit of every element and shift them
			// x << 0 | y << 1 | z << 2 | w << 3
			// so if every element matches, the result should be 0xf (1111, first four bits set)
			int r = _mm_movemask_ps(z);
			return r == 0xf;
		}

		inline bool VectorIsEqual(CBVVector v1, CBVVector v2, const float epsilon)
		{
			return VectorIsZero(_mm_sub_ps(v1, v2), epsilon);
		}

		inline BVVector VectorNegate(CBVVector v)
		{
			return _mm_xor_ps(v, _mm_set1_ps(-0.0f));
		}

		inline BVVector VectorZero()
		{
			return _mm_setzero_ps();
		}

		inline BVVector VectorReplicate(const float s)
		{
			return _mm_set1_ps(s);
		}

		inline BVVector VectorReplicateX(CBVVector v)
		{
			return _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
		}

		inline BVVector VectorReplicateY(CBVVector v)
		{
			return _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
		}

		inline BVVector VectorReplicateZ(CBVVector v)
		{
			return _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
		}

		inline BVVector VectorReplicateW(CBVVector v)
		{
			return _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
		}

		inline BVVector VectorMax(CBVVector v1, CBVVector v2)
		{
			return _mm_max_ps(v1, v2);
		}

		inline BVVector VectorMin(CBVVector v1, CBVVector v2)
		{
			return _mm_min_ps(v1, v2);
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
			// _mm_cvtss_f32 gets the value of the lowest float, in this case, X
			// _mm_store_ss could also be used, but I think it might be slower
			return _mm_cvtss_f32(VectorDotV(v1, v2));
		}

		inline BVVector VectorDotV(CBVVector v1, CBVVector v2)
		{
			// _mm_dp_ps will use a 8 bit mask where:
			// bits 7, 6, 5 and 4 describe which elements to multiply
			// bits 3, 2, 1 and 0 describe where elements will be stored

			// For this implementation, 0111 1111 will be used, so x, y and z will get multiplied
			// and the value is stored across all elements
			return _mm_dp_ps(v1, v2, 0x7F);
		}

		inline BVVector VectorCross(CBVVector v1, CBVVector v2)
		{
			// Using _MM_SHUFFLE macro to change the element places (reverse order)
			// A(3, 2, 1, 0)
			// B(3, 2, 1, 0)
			// SHUFFLE AB (0, 1, 2, 3) = (B0, B1, A2, A3) (reverse)
			// Result => (A.w, A.z, B.y, B.x)

			// (v1.y, v1.z, v1.x, v1.w) * (v2.z, v2.x, v2.y, v2.w)
			// -
			// (v1.z, v1.x, v1.y, v1.w) * (v2.y, v2.z, v2.x, v2.w)
			__m128i w0 = _mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
			BVVector r = _mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(_mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1)))
				);

			return _mm_and_ps(r, _mm_castsi128_ps(w0));
		}

		inline float VectorLengthSqr(CBVVector v)
		{
			return _mm_cvtss_f32(VectorLengthSqrV(v));
		}

		inline BVVector VectorLengthSqrV(CBVVector v)
		{
			return VectorDotV(v, v);
		}

		inline float VectorLength(CBVVector v)
		{
			return _mm_cvtss_f32(VectorLengthV(v));
		}

		inline BVVector VectorLengthV(CBVVector v)
		{
			return _mm_sqrt_ps(VectorLengthSqrV(v));
		}

		inline BVVector VectorNormalize(CBVVector v)
		{
			return _mm_mul_ps(v, _mm_rsqrt_ps(VectorLengthSqrV(v)));
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
			BVVector vT = _mm_set1_ps(t);
			BVVector v2MinusV1 = _mm_sub_ps(v2, v1);

			return _mm_add_ps(v1, _mm_mul_ps(vT, v2MinusV1));
		}

		inline BVVector VectorReflect(CBVVector i, CBVVector n)
		{
			// Reflection = I - 2(I.N)N
			BVVector twoTimesProj = VectorProject(i, n);
			twoTimesProj = VectorAdd(twoTimesProj, twoTimesProj);

			return VectorSub(i, twoTimesProj);
		}

		inline BVVector VectorTransformDir(CBVVector v, CRBVMatrix m)
		{
			BVVector c = VectorReplicateX(v);
			BVVector r = _mm_mul_ps(c, m.r[0]);

			c = VectorReplicateY(v);
			r = _mm_add_ps(r, _mm_mul_ps(c, m.r[1]));

			c = VectorReplicateZ(v);
			r = _mm_add_ps(r, _mm_mul_ps(c, m.r[2]));

			return r;
		}

		inline BVVector VectorTransformPoint(CBVVector v, CRBVMatrix m)
		{
			BVVector c = VectorReplicateX(v);
			BVVector r = _mm_mul_ps(c, m.r[0]);

			c = VectorReplicateY(v);
			r = _mm_add_ps(r, _mm_mul_ps(c, m.r[1]));

			c = VectorReplicateZ(v);
			r = _mm_add_ps(r, _mm_mul_ps(c, m.r[2]));

			return _mm_add_ps(r, m.r[3]);
		}
	}
}


#endif