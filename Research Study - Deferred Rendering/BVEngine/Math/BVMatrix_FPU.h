#ifndef BVMATRIX_FPU_H
#define BVMATRIX_FPU_H

#include "BVVector_FPU.h"

namespace BVMath
{
	namespace FPU
	{
		// ==================================
		// Matrix operations
		// ==================================

		// ======================
		// Declarations
		// ======================

		// ======================
		// Access
		// ======================

		BVMatrix MatrixFromFloat3x3Ptr(const float * const p);
		BVMatrix MatrixFromFloat4x4Ptr(const float * const p);

		BVMatrix MatrixFromFloat3x3(const BVFloat33 & m);
		BVMatrix MatrixFromFloat4x4(const BVFloat44 & m);

		void MatrixToFloat3x3Ptr(CRBVMatrix m, float * const p);
		void MatrixToFloat4x4Ptr(CRBVMatrix m, float * const p);

		void MatrixToFloat3x3(CRBVMatrix m, BVFloat33 & r);
		void MatrixToFloat4x4(CRBVMatrix m, BVFloat44 & r);


		// ======================
		// Basic Operations
		// ======================

		BVMatrix MatrixAdd(CRBVMatrix m1, CRBVMatrix m2);
		BVMatrix MatrixSub(CRBVMatrix m1, CRBVMatrix m2);

		BVMatrix MatrixMul(CRBVMatrix m1, CRBVMatrix m2);

		BVVector MatrixMul(CRBVMatrix m, CBVVector v); // Column-major

		BVMatrix MatrixDiv(CRBVMatrix m, const float s);

		BVMatrix MatrixIdentity();

		BVMatrix MatrixTranspose(CRBVMatrix m);

		BVVector MatrixDeterminantV(CRBVMatrix m);
		float MatrixDeterminant(CRBVMatrix m);

		BVMatrix MatrixInverse(CRBVMatrix m);

		// ======================
		// Operators
		// ======================

		BVMatrix operator + (CRBVMatrix m);
		BVMatrix operator - (CRBVMatrix m);

		BVMatrix operator + (CRBVMatrix m1, CRBVMatrix m2);
		BVMatrix & operator += (RBVMatrix m1, CRBVMatrix m2);

		BVMatrix operator - (CRBVMatrix m1, CRBVMatrix m2);
		BVMatrix & operator -= (RBVMatrix m1, CRBVMatrix m2);

		BVMatrix operator * (CRBVMatrix m1, CRBVMatrix m2);
		BVMatrix & operator *= (RBVMatrix m1, CRBVMatrix m2);

		BVVector operator * (CRBVMatrix m, CBVVector v);
		BVVector & operator *= (CRBVMatrix m, RBVVector v);

		BVMatrix operator * (CRBVMatrix m, const float s);
		BVMatrix & operator *= (RBVMatrix m, const float s);

		BVMatrix operator / (CRBVMatrix m, const float s);
		BVMatrix & operator /= (RBVMatrix m, const float s);

		// ======================
		// Specialized Operations
		// ======================

		BVMatrix MatrixWorldInvTranspose(CRBVMatrix m);

		BVMatrix MatrixScaling(const float sX, const float sY, const float sZ);

		BVMatrix MatrixTranslation(const float tX, const float tY, const float tZ);

		BVMatrix MatrixRotationX(const float angle);
		BVMatrix MatrixRotationY(const float angle);
		BVMatrix MatrixRotationZ(const float angle);
		BVMatrix MatrixRotationAxis(CBVVector axis, const float angle);

		BVMatrix MatrixLookAt(CBVVector eyePos, CBVVector dirVec, CBVVector upVec);
		BVMatrix MatrixLookAtLH(CBVVector eyePos, CBVVector lookPos, CBVVector upVec);
		BVMatrix MatrixLookAtRH(CBVVector eyePos, CBVVector lookPos, CBVVector upVec);

		BVMatrix MatrixPerspectiveLH_DX(const float nearZ, const float farZ, const float aspectRatio, const float fovY);
		BVMatrix MatrixPerspectiveRH_DX(const float nearZ, const float farZ, const float aspectRatio, const float fovY);

		BVMatrix MatrixPerspectiveLH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY);
		BVMatrix MatrixPerspectiveRH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY);

		BVMatrix MatrixOrthographicLH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicRH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicLH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicRH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		// ======================
		// Definitions
		// ======================

		// ======================
		// Access
		// ======================

		inline BVMatrix MatrixFromFloat3x3Ptr(const float * const p)
		{
			BVMatrix r;
			r.r[0] = VectorFromFloat3Ptr(p);
			r.r[1] = VectorFromFloat3Ptr(p + 3);
			r.r[2] = VectorFromFloat3Ptr(p + 6);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixFromFloat4x4Ptr(const float * const p)
		{
			BVMatrix r;
			r.r[0] = VectorFromFloat4Ptr(p);
			r.r[1] = VectorFromFloat4Ptr(p + 4);
			r.r[2] = VectorFromFloat4Ptr(p + 8);
			r.r[3] = VectorFromFloat4Ptr(p + 12);

			return r;
		}

		inline BVMatrix MatrixFromFloat3x3(const BVFloat33 & m)
		{
			return MatrixFromFloat3x3Ptr(&m.r[0].x);
		}

		inline BVMatrix MatrixFromFloat4x4(const BVFloat44 & m)
		{
			return MatrixFromFloat4x4Ptr(&m.r[0].x);
		}

		inline void MatrixToFloat3x3Ptr(CRBVMatrix m, float * const p)
		{
			VectorToFloat3Ptr(m.r[0], p);
			VectorToFloat3Ptr(m.r[1], p + 3);
			VectorToFloat3Ptr(m.r[2], p + 6);
		}

		inline void MatrixToFloat4x4Ptr(CRBVMatrix m, float * const p)
		{
			VectorToFloat4Ptr(m.r[0], p);
			VectorToFloat4Ptr(m.r[1], p + 4);
			VectorToFloat4Ptr(m.r[2], p + 8);
			VectorToFloat4Ptr(m.r[3], p + 12);
		}

		inline void MatrixToFloat3x3(CRBVMatrix m, BVFloat33 & r)
		{
			MatrixToFloat3x3Ptr(m, &r.r[0].x);
		}

		inline void MatrixToFloat4x4(CRBVMatrix m, BVFloat44 & r)
		{
			MatrixToFloat4x4Ptr(m, &r.r[0].x);
		}


		// ======================
		// Basic Operations
		// ======================

		inline BVMatrix MatrixAdd(CRBVMatrix m1, CRBVMatrix m2)
		{
			BVMatrix m;
			m.r[0] = VectorAdd(m1.r[0], m2.r[0]);
			m.r[1] = VectorAdd(m1.r[1], m2.r[1]);
			m.r[2] = VectorAdd(m1.r[2], m2.r[2]);
			m.r[3] = VectorAdd(m1.r[3], m2.r[3]);

			return m;
		}

		inline BVMatrix MatrixSub(CRBVMatrix m1, CRBVMatrix m2)
		{
			BVMatrix m;
			m.r[0] = VectorSub(m1.r[0], m2.r[0]);
			m.r[1] = VectorSub(m1.r[1], m2.r[1]);
			m.r[2] = VectorSub(m1.r[2], m2.r[2]);
			m.r[3] = VectorSub(m1.r[3], m2.r[3]);

			return m;
		}

		inline BVMatrix MatrixMul(CRBVMatrix m1, CRBVMatrix m2)
		{
			BVMatrix m;
			m.r[0] = VectorSet(
					m1.r[0].x * m2.r[0].x + m1.r[0].y * m2.r[1].x + m1.r[0].z * m2.r[2].x + m1.r[0].w * m2.r[3].x,
					m1.r[0].x * m2.r[0].y + m1.r[0].y * m2.r[1].y + m1.r[0].z * m2.r[2].y + m1.r[0].w * m2.r[3].y,
					m1.r[0].x * m2.r[0].z + m1.r[0].y * m2.r[1].z + m1.r[0].z * m2.r[2].z + m1.r[0].w * m2.r[3].z,
					m1.r[0].x * m2.r[0].w + m1.r[0].y * m2.r[1].w + m1.r[0].z * m2.r[2].w + m1.r[0].w * m2.r[3].w
				);

			m.r[1] = VectorSet(
				m1.r[1].x * m2.r[0].x + m1.r[1].y * m2.r[1].x + m1.r[1].z * m2.r[2].x + m1.r[1].w * m2.r[3].x,
				m1.r[1].x * m2.r[0].y + m1.r[1].y * m2.r[1].y + m1.r[1].z * m2.r[2].y + m1.r[1].w * m2.r[3].y,
				m1.r[1].x * m2.r[0].z + m1.r[1].y * m2.r[1].z + m1.r[1].z * m2.r[2].z + m1.r[1].w * m2.r[3].z,
				m1.r[1].x * m2.r[0].w + m1.r[1].y * m2.r[1].w + m1.r[1].z * m2.r[2].w + m1.r[1].w * m2.r[3].w
			);

			m.r[2] = VectorSet(
				m1.r[2].x * m2.r[0].x + m1.r[2].y * m2.r[1].x + m1.r[2].z * m2.r[2].x + m1.r[2].w * m2.r[3].x,
				m1.r[2].x * m2.r[0].y + m1.r[2].y * m2.r[1].y + m1.r[2].z * m2.r[2].y + m1.r[2].w * m2.r[3].y,
				m1.r[2].x * m2.r[0].z + m1.r[2].y * m2.r[1].z + m1.r[2].z * m2.r[2].z + m1.r[2].w * m2.r[3].z,
				m1.r[2].x * m2.r[0].w + m1.r[2].y * m2.r[1].w + m1.r[2].z * m2.r[2].w + m1.r[2].w * m2.r[3].w
			);

			m.r[1] = VectorSet(
				m1.r[3].x * m2.r[0].x + m1.r[3].y * m2.r[1].x + m1.r[3].z * m2.r[2].x + m1.r[3].w * m2.r[3].x,
				m1.r[3].x * m2.r[0].y + m1.r[3].y * m2.r[1].y + m1.r[3].z * m2.r[2].y + m1.r[3].w * m2.r[3].y,
				m1.r[3].x * m2.r[0].z + m1.r[3].y * m2.r[1].z + m1.r[3].z * m2.r[2].z + m1.r[3].w * m2.r[3].z,
				m1.r[3].x * m2.r[0].w + m1.r[3].y * m2.r[1].w + m1.r[3].z * m2.r[2].w + m1.r[3].w * m2.r[3].w
			);

			return m;
		}

		inline BVVector MatrixMul(CRBVMatrix m, CBVVector v)
		{
			return VectorSet(
				m.r[0].x * v.x + m.r[0].y * v.y + m.r[0].z * v.z + m.r[0].w * v.w,
				m.r[1].x * v.x + m.r[1].y * v.y + m.r[1].z * v.z + m.r[1].w * v.w,
				m.r[2].x * v.x + m.r[2].y * v.y + m.r[2].z * v.z + m.r[2].w * v.w,
				m.r[3].x * v.x + m.r[3].y * v.y + m.r[3].z * v.z + m.r[3].w * v.w
				);
		}

		inline BVMatrix MatrixDiv(CRBVMatrix m, const float s)
		{
			BVMatrix r;
			r.r[0] = VectorDiv(m.r[0], s);
			r.r[1] = VectorDiv(m.r[1], s);
			r.r[2] = VectorDiv(m.r[2], s);
			r.r[3] = VectorDiv(m.r[3], s);

			return r;
		}

		inline BVMatrix MatrixIdentity()
		{
			BVMatrix m;
			m.r[0] = VectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			m.r[1] = VectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			m.r[2] = VectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			m.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return m;
		}

		inline BVMatrix MatrixTranspose(CRBVMatrix m)
		{
			BVMatrix mT;
			mT.r[0] = VectorSet(m.r[0].x, m.r[1].x, m.r[2].x, m.r[3].x);
			mT.r[1] = VectorSet(m.r[0].y, m.r[1].y, m.r[2].y, m.r[3].y);
			mT.r[2] = VectorSet(m.r[0].z, m.r[1].z, m.r[2].z, m.r[3].z);
			mT.r[3] = VectorSet(m.r[0].w, m.r[1].w, m.r[2].w, m.r[3].w);

			return mT;
		}

		inline BVVector MatrixDeterminantV(CRBVMatrix m)
		{
			float a1 = m.r[2].z * m.r[3].w - m.r[2].w * m.r[3].z;
			float a2 = m.r[2].y * m.r[3].w - m.r[2].w * m.r[3].y;
			float a3 = m.r[2].y * m.r[3].z - m.r[2].z * m.r[3].y;
			float a4 = m.r[2].x * m.r[3].w - m.r[2].w * m.r[3].x;
			float a5 = m.r[2].x * m.r[3].z - m.r[2].z * m.r[3].x;
			float a6 = m.r[2].x * m.r[3].y - m.r[2].y * m.r[3].x;

			float det =
				  m.r[0].x * (m.r[1].y * (a1) - m.r[1].z * (a2) + m.r[1].w * (a3))
				- m.r[0].y * (m.r[1].x * (a1) - m.r[1].z * (a4) + m.r[1].w * (a5))
				+ m.r[0].z * (m.r[1].x * (a2) - m.r[1].y * (a4) + m.r[1].w * (a6))
				- m.r[0].w * (m.r[1].x * (a3) - m.r[1].y * (a5) + m.r[1].z * (a6));

			return VectorSet(det, det, det, det);
		}

		inline float MatrixDeterminant(CRBVMatrix m)
		{
			return MatrixDeterminantV(m).x;
		}

		inline BVMatrix MatrixInverse(CRBVMatrix m)
		{
			BVMatrix mT = MatrixTranspose(m);

			// First 12 coefficients
			float tmp[12];
			tmp[0] =  mT.r[2].z * mT.r[3].w;
			tmp[1] =  mT.r[2].w * mT.r[3].z;
			tmp[2] =  mT.r[2].y * mT.r[3].w;
			tmp[3] =  mT.r[2].w * mT.r[3].y;
			tmp[4] =  mT.r[2].y * mT.r[3].z;
			tmp[5] =  mT.r[2].z * mT.r[3].y;
			tmp[6] =  mT.r[2].x * mT.r[3].w;
			tmp[7] =  mT.r[2].w * mT.r[3].x;
			tmp[8] =  mT.r[2].x * mT.r[3].z;
			tmp[9] =  mT.r[2].z * mT.r[3].x;
			tmp[10] = mT.r[2].x * mT.r[3].y;
			tmp[11] = mT.r[2].y * mT.r[3].x;

			BVMatrix r;
			r.r[0].x =  tmp[0] * mT.r[1].y + tmp[3] * mT.r[1].z + tmp[4] *  mT.r[1].w;
			r.r[0].x -= tmp[1] * mT.r[1].y + tmp[2] * mT.r[1].z + tmp[5] *  mT.r[1].w;
			r.r[0].y =  tmp[1] * mT.r[1].x + tmp[6] * mT.r[1].z + tmp[9] *  mT.r[1].w;
			r.r[0].y -= tmp[0] * mT.r[1].x + tmp[7] * mT.r[1].z + tmp[8] *  mT.r[1].w;
			r.r[0].z =  tmp[2] * mT.r[1].x + tmp[7] * mT.r[1].y + tmp[10] * mT.r[1].w;
			r.r[0].z -= tmp[3] * mT.r[1].x + tmp[6] * mT.r[1].y + tmp[11] * mT.r[1].w;
			r.r[0].w =  tmp[5] * mT.r[1].x + tmp[8] * mT.r[1].y + tmp[11] * mT.r[1].z;
			r.r[0].w -= tmp[4] * mT.r[1].x + tmp[9] * mT.r[1].y + tmp[10] * mT.r[1].z;
			r.r[1].x =  tmp[1] * mT.r[0].y + tmp[2] * mT.r[0].z + tmp[5] *  mT.r[0].w;
			r.r[1].x -= tmp[0] * mT.r[0].y + tmp[3] * mT.r[0].z + tmp[4] *  mT.r[0].w;
			r.r[1].y =  tmp[0] * mT.r[0].x + tmp[7] * mT.r[0].z + tmp[8] *  mT.r[0].w;
			r.r[1].y -= tmp[1] * mT.r[0].x + tmp[6] * mT.r[0].z + tmp[9] *  mT.r[0].w;
			r.r[1].z =  tmp[3] * mT.r[0].x + tmp[6] * mT.r[0].y + tmp[11] * mT.r[0].w;
			r.r[1].z -= tmp[2] * mT.r[0].x + tmp[7] * mT.r[0].y + tmp[10] * mT.r[0].w;
			r.r[1].w =  tmp[4] * mT.r[0].x + tmp[9] * mT.r[0].y + tmp[10] * mT.r[0].z;
			r.r[1].w -= tmp[5] * mT.r[0].x + tmp[8] * mT.r[0].y + tmp[11] * mT.r[0].z;

			// Second 12 coefficients
			tmp[0] =  mT.r[0].z * mT.r[1].w;
			tmp[1] =  mT.r[0].w * mT.r[1].z;
			tmp[2] =  mT.r[0].y * mT.r[1].w;
			tmp[3] =  mT.r[0].w * mT.r[1].y;
			tmp[4] =  mT.r[0].y * mT.r[1].z;
			tmp[5] =  mT.r[0].z * mT.r[1].y;
			tmp[6] =  mT.r[0].x * mT.r[1].w;
			tmp[7] =  mT.r[0].w * mT.r[1].x;
			tmp[8] =  mT.r[0].x * mT.r[1].z;
			tmp[9] =  mT.r[0].z * mT.r[1].x;
			tmp[10] = mT.r[0].x * mT.r[1].y;
			tmp[11] = mT.r[0].y * mT.r[1].x;

			r.r[2].x =  tmp[0]  * mT.r[3].y + tmp[3] *  mT.r[3].z + tmp[4] *  mT.r[3].w;
			r.r[2].x -= tmp[1]  * mT.r[3].y + tmp[2] *  mT.r[3].z + tmp[5] *  mT.r[3].w;
			r.r[2].y =  tmp[1]  * mT.r[3].x + tmp[6] *  mT.r[3].z + tmp[9] *  mT.r[3].w;
			r.r[2].y -= tmp[0]  * mT.r[3].x + tmp[7] *  mT.r[3].z + tmp[8] *  mT.r[3].w;
			r.r[2].z =  tmp[2]  * mT.r[3].x + tmp[7] *  mT.r[3].y + tmp[10] * mT.r[3].w;
			r.r[2].z -= tmp[3]  * mT.r[3].x + tmp[6] *  mT.r[3].y + tmp[11] * mT.r[3].w;
			r.r[2].w =  tmp[5]  * mT.r[3].x + tmp[8] *  mT.r[3].y + tmp[11] * mT.r[3].z;
			r.r[2].w -= tmp[4]  * mT.r[3].x + tmp[9] *  mT.r[3].y + tmp[10] * mT.r[3].z;
			r.r[3].x =  tmp[2]  * mT.r[2].z + tmp[5] *  mT.r[2].w + tmp[1] *  mT.r[2].y;
			r.r[3].x -= tmp[4]  * mT.r[2].w + tmp[0] *  mT.r[2].y + tmp[3] *  mT.r[2].z;
			r.r[3].y =  tmp[8]  * mT.r[2].w + tmp[0] *  mT.r[2].x + tmp[7] *  mT.r[2].z;
			r.r[3].y -= tmp[6]  * mT.r[2].z + tmp[9] *  mT.r[2].w + tmp[1] *  mT.r[2].x;
			r.r[3].z =  tmp[6]  * mT.r[2].y + tmp[11] * mT.r[2].w + tmp[3] *  mT.r[2].x;
			r.r[3].z -= tmp[10] * mT.r[2].w + tmp[2] *  mT.r[2].x + tmp[7] *  mT.r[2].y;
			r.r[3].w =  tmp[10] * mT.r[2].z + tmp[4] *  mT.r[2].x + tmp[9] *  mT.r[2].y;
			r.r[3].w -= tmp[8]  * mT.r[2].y + tmp[11] * mT.r[2].z + tmp[5] *  mT.r[2].x;

			float det = 1.0f / (mT.r[0].x * r.r[0].x + mT.r[0].y * r.r[0].y + mT.r[0].z * r.r[0].z + mT.r[0].w * r.r[0].w);

			r.r[0].x *= det;
			r.r[0].y *= det;
			r.r[0].z *= det;
			r.r[0].w *= det;
			r.r[1].x *= det;
			r.r[1].y *= det;
			r.r[1].z *= det;
			r.r[1].w *= det;
			r.r[2].x *= det;
			r.r[2].y *= det;
			r.r[2].z *= det;
			r.r[2].w *= det;
			r.r[3].x *= det;
			r.r[3].y *= det;
			r.r[3].z *= det;
			r.r[3].w *= det;
		}

		// ======================
		// Operators
		// ======================

		inline BVMatrix operator + (CRBVMatrix m)
		{
			return m;
		}

		inline BVMatrix operator - (CRBVMatrix m)
		{
			BVMatrix r;
			r.r[0] = VectorNegate(m.r[0]);
			r.r[1] = VectorNegate(m.r[1]);
			r.r[2] = VectorNegate(m.r[2]);
			r.r[3] = VectorNegate(m.r[3]);

			return r;
		}

		inline BVMatrix operator + (CRBVMatrix m1, CRBVMatrix m2)
		{
			return MatrixAdd(m1, m2);
		}

		inline BVMatrix & operator += (RBVMatrix m1, CRBVMatrix m2)
		{
			m1 = MatrixAdd(m1, m2);
			return m1;
		}

		inline BVMatrix operator - (CRBVMatrix m1, CRBVMatrix m2)
		{
			return MatrixSub(m1, m2);
		}

		inline BVMatrix & operator -= (RBVMatrix m1, CRBVMatrix m2)
		{
			m1 = MatrixSub(m1, m2);
			return m1;
		}

		inline BVMatrix operator * (CRBVMatrix m1, CRBVMatrix m2)
		{
			return MatrixMul(m1, m2);
		}

		inline BVMatrix & operator *= (RBVMatrix m1, CRBVMatrix m2)
		{
			m1 = MatrixMul(m1, m2);
			return m1;
		}

		inline BVVector operator * (CRBVMatrix m, CBVVector v)
		{
			return MatrixMul(m, v);
		}

		inline BVVector & operator *= (CRBVMatrix m, RBVVector v)
		{
			v = MatrixMul(m, v);
			return v;
		}

		inline BVMatrix operator * (CRBVMatrix m, const float s)
		{
			BVVector v = VectorReplicate(s);

			BVMatrix r;
			r.r[0] = VectorMul(m.r[0], v);
			r.r[1] = VectorMul(m.r[1], v);
			r.r[2] = VectorMul(m.r[2], v);
			r.r[3] = VectorMul(m.r[3], v);

			return r;
		}

		inline BVMatrix & operator *= (RBVMatrix m, const float s)
		{
			m.r[0] = VectorMul(m.r[0], s);
			m.r[1] = VectorMul(m.r[1], s);
			m.r[2] = VectorMul(m.r[2], s);
			m.r[3] = VectorMul(m.r[3], s);

			return m;
		}

		inline BVMatrix operator / (CRBVMatrix m, const float s)
		{
			return MatrixDiv(m, s);
		}

		inline BVMatrix & operator /= (RBVMatrix m, const float s)
		{
			m = MatrixDiv(m, s);
			return m;
		}

		// ======================
		// Specialized Operations
		// ======================

		inline BVMatrix MatrixWorldInvTranspose(CRBVMatrix m)
		{
			BVMatrix r = m;
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return MatrixTranspose(MatrixInverse(r));
		}

		inline BVMatrix MatrixScaling(const float sX, const float sY, const float sZ)
		{
			BVMatrix r;
			r.r[0] = VectorSet(sX, 0.0f, 0.0f, 0.0f);
			r.r[1] = VectorSet(0.0f, sY, 0.0f, 0.0f);
			r.r[2] = VectorSet(0.0f, 0.0f, sZ, 0.0f);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixTranslation(const float tX, const float tY, const float tZ)
		{
			BVMatrix r;
			r.r[0] = VectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			r.r[1] = VectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			r.r[2] = VectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			r.r[3] = VectorSet(tX, tY, tZ, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationX(const float angle)
		{
			float cosSin[] = { cosf(angle), sinf(angle) };

			BVMatrix r;
			r.r[0] = VectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			r.r[1] = VectorSet(0.0f, cosSin[0], cosSin[1], 0.0f);
			r.r[2] = VectorSet(0.0f, -cosSin[1], cosSin[0], 0.0f);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationY(const float angle)
		{
			float cosSin[] = { cosf(angle), sinf(angle) };

			BVMatrix r;
			r.r[0] = VectorSet(cosSin[0], 0.0f, -cosSin[1], 0.0f);
			r.r[1] = VectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			r.r[2] = VectorSet(cosSin[1], 0.0f, cosSin[0], 0.0f);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationZ(const float angle)
		{
			float cosSin[] = { cosf(angle), sinf(angle) };
			BVVector vCosSin = VectorFromFloat2Ptr(cosSin);
			cosSin[1] = -cosSin[1];
			BVVector vMinusSinCos = VectorFromFloat2Ptr(cosSin);

			BVMatrix r;
			r.r[0] = VectorSet(cosSin[0], cosSin[1], 0.0f, 0.0f);
			r.r[1] = VectorSet(-cosSin[1], cosSin[0], 0.0f, 0.0f);
			r.r[2] = VectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationAxis(CBVVector axis, const float angle)
		{
			float cosAngle = cosf(angle);
			float sinAngle = sinf(angle);
			float oneMinusCos = 1.0f - cosAngle;

			BVMatrix r;
			r.r[0].x = cosAngle + axis.x * axis.x * oneMinusCos;
			r.r[0].y = axis.y * axis.x * oneMinusCos + axis.z * sinAngle;
			r.r[0].z = axis.z * axis.x * oneMinusCos - axis.y * sinAngle;
			r.r[0].w = 0.0f;

			r.r[1].x = axis.x * axis.y * oneMinusCos - axis.z * sinAngle;
			r.r[1].y = cosAngle + axis.y * axis.y * oneMinusCos;
			r.r[1].z = axis.z * axis.y * oneMinusCos + axis.x * sinAngle;
			r.r[1].w = 0.0f;

			r.r[3].x = axis.x * axis.z * oneMinusCos + axis.y * sinAngle;
			r.r[3].y = axis.y * axis.z * oneMinusCos - axis.x * sinAngle;
			r.r[3].z = cosAngle + axis.z * axis.z * oneMinusCos;
			r.r[3].w = 0.0f;

			r.r[3].x = r.r[3].y = r.r[3].z = 0.0f;
			r.r[3].w = 1.0f;

			return r;
		}

		inline BVMatrix MatrixLookAt(CBVVector eyePos, CBVVector dirVec, CBVVector upVec)
		{
			BVVector z = VectorNormalize(dirVec);

			BVVector x = VectorCross(upVec, z);
			x = VectorNormalize(x);

			BVVector y = VectorCross(z, x);

			BVVector w = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			BVMatrix r(x, y, z, w);

			r = MatrixTranspose(r);
			r.r[3] = VectorSet(-VectorDot(x, eyePos), -VectorDot(y, eyePos), -VectorDot(z, eyePos), 1.0f);

			return r;
		}

		inline BVMatrix MatrixLookAtLH(CBVVector eyePos, CBVVector lookPos, CBVVector upVec)
		{
			BVVector dir = VectorSub(lookPos, eyePos);
			return MatrixLookAt(eyePos, dir, upVec);
		}

		inline BVMatrix MatrixLookAtRH(CBVVector eyePos, CBVVector lookPos, CBVVector upVec)
		{
			BVVector dir = VectorSub(eyePos, lookPos);
			return MatrixLookAt(eyePos, dir, upVec);
		}

		inline BVMatrix MatrixPerspectiveLH_DX(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ / (farZ - nearZ);

			const float vals[] = { a / aspectRatio, a, b, 1.0f, -nearZ * b};

			BVMatrix r;
			r.r[0].x = vals[0];
			r.r[1].y = vals[1];
			r.r[2].z = vals[2];
			r.r[2].w = vals[3];
			r.r[3].z = vals[4];

			return r;
		}

		inline BVMatrix MatrixPerspectiveRH_DX(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ / (farZ - nearZ);

			const float vals[] = { a / aspectRatio, a, b, -1.0f, nearZ * b };

			BVMatrix r;
			r.r[0].x = vals[0];
			r.r[1].y = vals[1];
			r.r[2].z = vals[2];
			r.r[2].w = vals[3];
			r.r[3].z = vals[4];

			return r;
		}

		inline BVMatrix MatrixPerspectiveLH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ - nearZ;

			const float vals[] = { a / aspectRatio, a, -(farZ + nearZ) / b, 1.0f, (2.0f * farZ * nearZ) / b };

			BVMatrix r;
			r.r[0].x = vals[0];
			r.r[1].y = vals[1];
			r.r[2].z = vals[2];
			r.r[2].w = vals[3];
			r.r[3].z = vals[4];

			return r;
		}

		inline BVMatrix MatrixPerspectiveRH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ - nearZ;

			const float vals[] = { a / aspectRatio, a, -(farZ + nearZ) / b, -1.0f, (-2.0f * farZ * nearZ) / b };

			BVMatrix r;
			r.r[0].x = vals[0];
			r.r[1].y = vals[1];
			r.r[2].z = vals[2];
			r.r[2].w = vals[3];
			r.r[3].z = vals[4];

			return r;
		}

		inline BVMatrix MatrixOrthographicLH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverFMinusN = 1.0f / (farZ - nearZ);

			BVMatrix r;
			r.r[0].x = oneOverRMinusL + oneOverRMinusL;
			r.r[1].y = oneOverTMinusB + oneOverTMinusB;
			r.r[2].z = oneOverFMinusN;
			r.r[3] = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB, -oneOverFMinusN * nearZ, 1.0f);

			return r;
		}

		inline BVMatrix MatrixOrthographicRH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverNMinusF = 1.0f / (nearZ - farZ);

			BVMatrix r;
			r.r[0].x = oneOverRMinusL + oneOverRMinusL;
			r.r[1].y = oneOverTMinusB + oneOverTMinusB;
			r.r[2].z = oneOverNMinusF;
			r.r[3] = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB, oneOverNMinusF * nearZ, 1.0f);

			return r;
		}

		inline BVMatrix MatrixOrthographicLH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverNMinusF = 1.0f / (nearZ - farZ);

			BVMatrix r;
			r.r[0].x = oneOverRMinusL + oneOverRMinusL;
			r.r[1].y = oneOverTMinusB + oneOverTMinusB;
			r.r[2].z = -2.0f * oneOverNMinusF;
			r.r[3] = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB, (nearZ + farZ) * oneOverNMinusF, 1.0f);

			return r;
		}

		inline BVMatrix MatrixOrthographicRH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverFMinusN = 1.0f / (farZ - nearZ);

			BVMatrix r;
			r.r[0].x = oneOverRMinusL + oneOverRMinusL;
			r.r[1].y = oneOverTMinusB + oneOverTMinusB;
			r.r[2].z = -2.0f * oneOverFMinusN;
			r.r[3] = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB, -(nearZ + farZ) * oneOverFMinusN, 1.0f);

			return r;
		}
	}
}

#endif