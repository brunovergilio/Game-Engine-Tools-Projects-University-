#ifndef BVQUATERNION_FPU_H
#define BVQUATERNION_FPU_H

#include "BVVector_FPU.h"

namespace BVMath
{
	namespace FPU
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

			return VectorSet(
				 q1.w * q2.x + q2.w * q1.x + q1.y * q2.z - q1.z * q2.y,
				 q1.w * q2.y + q2.w * q1.y + q1.z * q2.x - q1.x * q2.z,
				 q1.w * q2.z + q2.w * q1.z + q1.x * q2.y - q1.y * q2.x,
				 q1.w * q2.w - q2.x * q1.x - q1.y * q2.y - q1.z * q2.z
				);
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

			return VectorSet(
				 q1.w * q2.x + q2.w * q1.x + q2.y * q1.z - q2.z * q1.y,
				 q1.w * q2.y + q2.w * q1.y + q2.z * q1.x - q2.x * q1.z,
				 q1.w * q2.z + q2.w * q1.z + q2.x * q1.y - q2.y * q1.x,
				 q1.w * q2.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z
			);
		}

		inline BVQuat QuaternionConjugate(CBVQuat q)
		{
			return VectorSet(-q.x, -q.y, -q.z, q.w);
		}

		inline BVQuat QuaternionInverse(CBVQuat q)
		{
			BVQuat conj = QuaternionConjugate(q);
			float oneOverLengthSqr = 1.0f / (QuaternionLengthSqr(q));

			return BVQuat(
				conj.x * oneOverLengthSqr,
				conj.y * oneOverLengthSqr,
				conj.z * oneOverLengthSqr,
				conj.w * oneOverLengthSqr
				);
		}

		inline BVQuat QuaternionNormalize(CBVQuat q)
		{
			float oneOverLengthSqr = QuaternionLengthSqr(q);

			return BVQuat(
				q.x * oneOverLengthSqr,
				q.y * oneOverLengthSqr,
				q.z * oneOverLengthSqr,
				q.w * oneOverLengthSqr
			);
		}

		inline float QuaternionDot(CBVQuat q1, CBVQuat q2)
		{
			// _mm_cvtss_f32 gets the value of the lowest float, in this case, X
			// _mm_store_ss could also be used, but I think it might be slower
			return QuaternionDotV(q1, q2).x;
		}

		inline BVQuat QuaternionDotV(CBVQuat q1, CBVQuat q2)
		{
			return VectorReplicate(q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w);
		}

		inline float QuaternionLengthSqr(CBVQuat q)
		{
			return QuaternionLengthSqrV(q).x;
		}

		inline BVQuat QuaternionLengthSqrV(CBVQuat q)
		{
			return QuaternionDotV(q, q);
		}

		inline float QuaternionLength(CBVQuat v)
		{
			return QuaternionLengthV(v).x;
		}

		inline BVQuat QuaternionLengthV(CBVQuat v)
		{
			float oneOverLengthSqr = 1.0f / QuaternionLengthSqr(v);
			return BVQuat(v.x * oneOverLengthSqr, v.y * oneOverLengthSqr, v.z * oneOverLengthSqr, v.w * oneOverLengthSqr);
		}

		inline BVQuat QuaternionRotationAxis(CBVVector v, float angle)
		{
			float halfAngle = angle * 0.5f;
			float sinCos[] = { sinf(halfAngle), cosf(halfAngle) };

			return BVVector(v.x * sinCos[0], v.y * sinCos[0], v.z * sinCos[0], sinCos[1]);
		}

		inline BVVector QuaternionQVQC(CBVQuat q, CBVVector v)
		{
			float w2MinusQDotQ = q.w * q.w - VectorDot(q, q);
			float twoTimesQDotV = 2.0f * VectorDot(q, v);
			float wTimesTwo = 2.0f * q.w;
			BVVector qCrossV = VectorCross(q, v);

			return BVVector(w2MinusQDotQ * v.x + twoTimesQDotV * q.x + wTimesTwo * qCrossV.x,
				w2MinusQDotQ * v.y + twoTimesQDotV * q.y + wTimesTwo * qCrossV.y,
				w2MinusQDotQ * v.z + twoTimesQDotV * q.z + wTimesTwo * qCrossV.z);
		}

		inline BVVector QuaternionQCVQ(CBVQuat q, CBVVector v)
		{
			float w2MinusQDotQ = q.w * q.w - VectorDot(q, q);
			float twoTimesQDotV = 2.0f * VectorDot(q, v);
			float wTimesTwo = 2.0f * q.w;
			BVVector vCrossQ = VectorCross(v, q);

			return BVVector(w2MinusQDotQ * v.x + twoTimesQDotV * q.x + wTimesTwo * vCrossQ.x,
				w2MinusQDotQ * v.y + twoTimesQDotV * q.y + wTimesTwo * vCrossQ.y,
				w2MinusQDotQ * v.z + twoTimesQDotV * q.z + wTimesTwo * vCrossQ.z);
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
			float xy = q.x * q.y;
			float xz = q.x * q.z;
			float xw = q.x * q.w;

			float yz = q.x * q.z;
			float yw = q.x * q.w;

			float zw = q.x * q.w;

			float xx = q.x * q.x;
			float yy = q.y * q.y;
			float zz = q.z * q.z;

			BVMatrix r;
			r.r[0].x = 1.0f - 2.0f * (yy + zz);
			r.r[0].y = 2.0f * (xy + zw);
			r.r[0].z = 2.0f * (xz - yw);
			r.r[0].w = 0.0f;

			r.r[1].x = 2.0f * (xy - zw);
			r.r[1].y = 1.0f - 2.0f * (xx + zz);
			r.r[1].z = 2.0f * (yz + xw);
			r.r[1].w = 0.0f;

			r.r[2].x = 2.0f * (xz + yw);
			r.r[2].y = 2.0f * (yz - xw);
			r.r[2].z = 1.0f - 2.0f * (xx + yy);
			r.r[2].w = 0.0f;

			r.r[3].x = 0.0f;
			r.r[3].y = 0.0f;
			r.r[3].z = 0.0f;
			r.r[3].w = 1.0f;

			return r;
		}

		inline BVQuat QuaternionFromMatrix(CRBVMatrix m)
		{
			float trace = m.r[0].x + m.r[1].y + m.r[2].z;

			BVQuat q;
			if (trace > 0)
			{
				float s = sqrtf(trace + 1.0) * 2; // S=4*qw 
				q.w = 0.25 * s;
				q.x = (m.r[2].y - m.r[1].z) / s;
				q.y = (m.r[0].z - m.r[2].x) / s;
				q.z = (m.r[1].x - m.r[0].y) / s;
			}
			else if ((m.r[0].x > m.r[1].y) && (m.r[0].x > m.r[2].z))
			{
				float s = sqrtf(1.0 + m.r[0].x - m.r[1].y - m.r[2].z) * 2; // S=4*qx 
				q.w = (m.r[2].y - m.r[1].z) / s;
				q.x = 0.25 * s;
				q.y = (m.r[0].y + m.r[1].x) / s;
				q.z = (m.r[0].z + m.r[2].x) / s;
			}
			else if (m.r[1].y > m.r[2].z)
			{
				float s = sqrtf(1.0 + m.r[1].y - m.r[0].x - m.r[2].z) * 2; // S=4*qy
				q.w = (m.r[0].z - m.r[2].x) / s;
				q.x = (m.r[0].y + m.r[1].x) / s;
				q.y = 0.25 * s;
				q.z = (m.r[1].z + m.r[2].y) / s;
			}
			else
			{
				float s = sqrtf(1.0 + m.r[2].z - m.r[0].x - m.r[1].y) * 2; // S=4*qz
				q.w = (m.r[1].x - m.r[0].y) / s;
				q.x = (m.r[0].z + m.r[2].x) / s;
				q.y = (m.r[1].z + m.r[2].y) / s;
				q.z = 0.25 * s;
			}

			return QuaternionNormalize(q);
		}

		inline BVQuat QuaternionSlerp(CBVQuat q1, CBVQuat q2, const float t, const float epsilon)
		{
			float cosOmega = QuaternionDot(q1, q2);
			BVQuat qt(q2);
			if (cosOmega < 0.0f)
			{
				cosOmega = -cosOmega;
				qt = QuaternionConjugate(qt);
			}

			float omega = acos(cosOmega);

			float mult1 = 1.0f - t;
			float mult2 = t;
			if (omega > epsilon)
			{
				float oneOverSinOmega = 1.0f / sin(omega);
				mult1 = sin(mult1 * omega) * oneOverSinOmega;
				mult2 = sin(mult2 * omega) * oneOverSinOmega;
			}

			BVQuat r = q1 * mult1 + qt * mult2;

			return QuaternionNormalize(r);
		}

		inline float QuaternionAngle(CBVQuat q)
		{
			return 2.0f * acosf(q.w);
		}
	}
}

#endif