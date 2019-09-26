#ifndef BVMATRIX_SIMD_H
#define BVMATRIX_SIMD_H

#include "BVVector_SIMD.h"

namespace BVMath
{
	namespace SIMD
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

		BVMatrix MatrixOrthographicLH_DX(const float width, const float height, const float nearZ, const float farZ);

		BVMatrix MatrixOrthographicOffCenterLH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicOffCenterRH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicOffCenterLH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ);

		BVMatrix MatrixOrthographicOffCenterRH_GL(const float right, const float left, const float top, const float bottom, const float
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
			r.r[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

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
			m.r[0] = _mm_add_ps(m1.r[0], m2.r[0]);
			m.r[1] = _mm_add_ps(m1.r[1], m2.r[1]);
			m.r[2] = _mm_add_ps(m1.r[2], m2.r[2]);
			m.r[3] = _mm_add_ps(m1.r[3], m2.r[3]);

			return m;
		}

		inline BVMatrix MatrixSub(CRBVMatrix m1, CRBVMatrix m2)
		{
			BVMatrix m;
			m.r[0] = _mm_sub_ps(m1.r[0], m2.r[0]);
			m.r[1] = _mm_sub_ps(m1.r[1], m2.r[1]);
			m.r[2] = _mm_sub_ps(m1.r[2], m2.r[2]);
			m.r[3] = _mm_sub_ps(m1.r[3], m2.r[3]);

			return m;
		}

		inline BVMatrix MatrixMul(CRBVMatrix m1, CRBVMatrix m2)
		{
			BVMatrix m;

			// Row 0
			BVVector tmp0 = _mm_shuffle_ps(m1.r[0], m1.r[0], _MM_SHUFFLE(0, 0, 0, 0));
			tmp0 = _mm_mul_ps(tmp0, m2.r[0]);

			BVVector tmp1 = _mm_shuffle_ps(m1.r[0], m1.r[0], _MM_SHUFFLE(1, 1, 1, 1));
			tmp1 = _mm_mul_ps(tmp1, m2.r[1]);

			BVVector tmp2 = _mm_shuffle_ps(m1.r[0], m1.r[0], _MM_SHUFFLE(2, 2, 2, 2));
			tmp2 = _mm_mul_ps(tmp2, m2.r[2]);

			BVVector tmp3 = _mm_shuffle_ps(m1.r[0], m1.r[0], _MM_SHUFFLE(3, 3, 3, 3));
			tmp3 = _mm_mul_ps(tmp3, m2.r[3]);

			m.r[0] = _mm_add_ps(tmp0, tmp1);
			m.r[0] = _mm_add_ps(m.r[0], _mm_add_ps(tmp2, tmp3));

			// Row 1
			tmp0 = _mm_shuffle_ps(m1.r[1], m1.r[1], _MM_SHUFFLE(0, 0, 0, 0));
			tmp0 = _mm_mul_ps(tmp0, m2.r[0]);

			tmp1 = _mm_shuffle_ps(m1.r[1], m1.r[1], _MM_SHUFFLE(1, 1, 1, 1));
			tmp1 = _mm_mul_ps(tmp1, m2.r[1]);

			tmp2 = _mm_shuffle_ps(m1.r[1], m1.r[1], _MM_SHUFFLE(2, 2, 2, 2));
			tmp2 = _mm_mul_ps(tmp2, m2.r[2]);

			tmp3 = _mm_shuffle_ps(m1.r[1], m1.r[1], _MM_SHUFFLE(3, 3, 3, 3));
			tmp3 = _mm_mul_ps(tmp3, m2.r[3]);

			m.r[1] = _mm_add_ps(tmp0, tmp1);
			m.r[1] = _mm_add_ps(m.r[1], _mm_add_ps(tmp2, tmp3));

			// Row 2
			tmp0 = _mm_shuffle_ps(m1.r[2], m1.r[2], _MM_SHUFFLE(0, 0, 0, 0));
			tmp0 = _mm_mul_ps(tmp0, m2.r[0]);

			tmp1 = _mm_shuffle_ps(m1.r[2], m1.r[2], _MM_SHUFFLE(1, 1, 1, 1));
			tmp1 = _mm_mul_ps(tmp1, m2.r[1]);

			tmp2 = _mm_shuffle_ps(m1.r[2], m1.r[2], _MM_SHUFFLE(2, 2, 2, 2));
			tmp2 = _mm_mul_ps(tmp2, m2.r[2]);

			tmp3 = _mm_shuffle_ps(m1.r[2], m1.r[2], _MM_SHUFFLE(3, 3, 3, 3));
			tmp3 = _mm_mul_ps(tmp3, m2.r[3]);

			m.r[2] = _mm_add_ps(tmp0, tmp1);
			m.r[2] = _mm_add_ps(m.r[2], _mm_add_ps(tmp2, tmp3));

			// Row 3
			tmp0 = _mm_shuffle_ps(m1.r[3], m1.r[3], _MM_SHUFFLE(0, 0, 0, 0));
			tmp0 = _mm_mul_ps(tmp0, m2.r[0]);

			tmp1 = _mm_shuffle_ps(m1.r[3], m1.r[3], _MM_SHUFFLE(1, 1, 1, 1));
			tmp1 = _mm_mul_ps(tmp1, m2.r[1]);

			tmp2 = _mm_shuffle_ps(m1.r[3], m1.r[3], _MM_SHUFFLE(2, 2, 2, 2));
			tmp2 = _mm_mul_ps(tmp2, m2.r[2]);

			tmp3 = _mm_shuffle_ps(m1.r[3], m1.r[3], _MM_SHUFFLE(3, 3, 3, 3));
			tmp3 = _mm_mul_ps(tmp3, m2.r[3]);

			m.r[3] = _mm_add_ps(tmp0, tmp1);
			m.r[3] = _mm_add_ps(m.r[3], _mm_add_ps(tmp2, tmp3));

			return m;
		}

		inline BVVector MatrixMul(CRBVMatrix m, CBVVector v)
		{
			BVMatrix t = MatrixTranspose(m);
		
			t.r[0] = _mm_mul_ps(v, t.r[0]);
			t.r[1] = _mm_mul_ps(v, t.r[1]);
			t.r[2] = _mm_mul_ps(v, t.r[2]);
			t.r[3] = _mm_mul_ps(v, t.r[3]);
		
			BVVector r = _mm_add_ps(t.r[0], t.r[1]);
			r = _mm_add_ps(r, _mm_add_ps(t.r[2], t.r[3]));
		
			return r;
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
			BVVector id = _mm_set_ss(1.0f);

			BVMatrix m;
			m.r[0] = id;
			m.r[1] = _mm_shuffle_ps(id, id, _MM_SHUFFLE(1, 1, 0, 1));
			m.r[2] = _mm_shuffle_ps(id, id, _MM_SHUFFLE(1, 0, 1, 1));
			m.r[3] = _mm_shuffle_ps(id, id, _MM_SHUFFLE(0, 1, 1, 1));

			return m;
		}

		inline BVMatrix MatrixTranspose(CRBVMatrix m)
		{
			// top left 2x2 matrix
			BVVector upperBlock = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(1, 0, 1, 0));
			// bottom left 2x2 matrix
			BVVector lowerBlock = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(1, 0, 1, 0));

			BVMatrix mT;
			mT.r[0] = _mm_shuffle_ps(upperBlock, lowerBlock, _MM_SHUFFLE(2, 0, 2, 0));
			mT.r[1] = _mm_shuffle_ps(upperBlock, lowerBlock, _MM_SHUFFLE(3, 1, 3, 1));

			// top right 2x2 matrix
			upperBlock = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			// bottom right 2x2 matrix
			lowerBlock = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(3, 2, 3, 2));

			mT.r[2] = _mm_shuffle_ps(upperBlock, lowerBlock, _MM_SHUFFLE(2, 0, 2, 0));
			mT.r[3] = _mm_shuffle_ps(upperBlock, lowerBlock, _MM_SHUFFLE(3, 1, 3, 1));

			return mT;
		}

		inline BVVector MatrixDeterminantV(CRBVMatrix m)
		{
			// Calculate the first 12 2x2 determinant multiplications, to avoid repeating them later
			BVVector cf1 = _mm_mul_ps(_mm_shuffle_ps(m.r[2], m.r[2], _MM_SHUFFLE(3, 1, 3, 2)), _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(1, 3, 2, 3)));
			BVVector cf2 = _mm_mul_ps(_mm_shuffle_ps(m.r[2], m.r[2], _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(0, 3, 1, 2)));
			BVVector cf3 = _mm_mul_ps(_mm_shuffle_ps(m.r[2], m.r[2], _MM_SHUFFLE(1, 0, 2, 0)), _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(0, 1, 0, 2)));

			// Compute the first multiplication for the first row
			BVVector tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(3, 2, 1, 0));
			BVVector tmp2 = _mm_shuffle_ps(m.r[1], m.r[1], _MM_SHUFFLE(2, 3, 0, 1));

			BVVector row0 = _mm_mul_ps(tmp1, tmp2);

			// Compute the second multiplication and first subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the third multiplication and first addition for the first row
			tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(0, 1, 2, 3));
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(3, 1, 2, 0));
			tmp2 = _mm_shuffle_ps(m.r[1], m.r[1], _MM_SHUFFLE(1, 0, 3, 2));

			row0 = _mm_add_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the fourth multiplication and second subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the fifth multiplication and second addition for the first row
			tmp1 = _mm_shuffle_ps(cf2, cf2, _MM_SHUFFLE(1, 3, 2, 0));
			tmp2 = _mm_shuffle_ps(m.r[1], m.r[1], _MM_SHUFFLE(0, 1, 2, 3));

			row0 = _mm_add_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the sixth multiplication and third subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 2, 3));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the determinant
			BVVector det = _mm_mul_ps(row0, m.r[0]);

			// Sum all values and replicate them through all elements
			det = _mm_add_ps(det, _mm_shuffle_ps(det, det, _MM_SHUFFLE(2, 3, 0, 1)));
			det = _mm_add_ps(det, _mm_shuffle_ps(det, det, _MM_SHUFFLE(1, 0, 3, 2)));

			return det;
		}

		inline float MatrixDeterminant(CRBVMatrix m)
		{
			return _mm_cvtss_f32(MatrixDeterminantV(m));
		}

		inline BVMatrix MatrixInverse(CRBVMatrix m)
		{
			BVMatrix mTmp = MatrixTranspose(m);

			// Calculate the first 12 2x2 determinant multiplications, to avoid repeating them later
			BVVector cf1 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(3, 1, 3, 2)), _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(1, 3, 2, 3)));
			BVVector cf2 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(0, 3, 1, 2)));
			BVVector cf3 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(1, 0, 2, 0)), _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(0, 1, 0, 2)));

			// Compute the first multiplication for the first row
			BVVector tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(3, 2, 1, 0));
			BVVector tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(2, 3, 0, 1));

			BVVector row0 = _mm_mul_ps(tmp1, tmp2);

			// Compute the first multiplication for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(2, 3, 0, 1));
			BVVector row1 = _mm_mul_ps(tmp1, tmp2);

			// Compute the second multiplication and first subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
			tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(2, 3, 0, 1));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the second multiplication and first subtraction for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(2, 3, 0, 1));

			row1 = _mm_sub_ps(_mm_mul_ps(tmp1, tmp2), row1);

			// Compute the third multiplication and first addition for the first row
			tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(0, 1, 2, 3));
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(3, 1, 2, 0));
			tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(1, 0, 3, 2));

			row0 = _mm_add_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the third multiplication and second subtraction for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(1, 0, 3, 2));

			row1 = _mm_sub_ps(row1, _mm_mul_ps(tmp1, tmp2));

			// Compute the fourth multiplication and second subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
			tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(1, 0, 3, 2));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the fourth multiplication and first addition for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(1, 0, 3, 2));

			row1 = _mm_add_ps(_mm_mul_ps(tmp1, tmp2), row1);

			// Compute the fifth multiplication and second addition for the first row
			tmp1 = _mm_shuffle_ps(cf2, cf2, _MM_SHUFFLE(1, 3, 2, 0));
			tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(0, 1, 2, 3));

			row0 = _mm_add_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the fifth multiplication and third subtraction for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(0, 1, 2, 3));

			row1 = _mm_sub_ps(row1, _mm_mul_ps(tmp1, tmp2));

			// Compute the sixth multiplication and third subtraction for the first row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 2, 3));
			tmp2 = _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(0, 1, 2, 3));

			row0 = _mm_sub_ps(row0, _mm_mul_ps(tmp1, tmp2));

			// Compute the sixth multiplication and second addition for the second row
			tmp2 = _mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(0, 1, 2, 3));

			row1 = _mm_add_ps(row1, _mm_mul_ps(tmp1, tmp2));

			// Calculate the second 12 2x2 determinant multiplications, avoid repetitions
			cf1 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(3, 1, 3, 2)), _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(1, 3, 2, 3)));
			cf2 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(0, 3, 1, 2)));
			cf3 = _mm_mul_ps(_mm_shuffle_ps(mTmp.r[0], mTmp.r[0], _MM_SHUFFLE(1, 0, 2, 0)), _mm_shuffle_ps(mTmp.r[1], mTmp.r[1], _MM_SHUFFLE(0, 1, 0, 2)));

			// Compute the first multiplication for the third row
			tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(3, 2, 1, 0));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(2, 3, 0, 1));

			BVVector row2 = _mm_mul_ps(tmp1, tmp2);

			// Compute the first multiplication for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(2, 3, 0, 1));
			BVVector row3 = _mm_mul_ps(tmp1, tmp2);

			// Compute the second multiplication and first subtraction for the third row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(2, 3, 0, 1));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(2, 3, 0, 1));

			row2 = _mm_sub_ps(row2, _mm_mul_ps(tmp1, tmp2));

			// Compute the second multiplication and first subtraction for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(2, 3, 0, 1));

			row3 = _mm_sub_ps(_mm_mul_ps(tmp1, tmp2), row3);

			// Compute the third multiplication and first addition for the third row
			tmp1 = _mm_shuffle_ps(cf1, cf3, _MM_SHUFFLE(0, 1, 2, 3));
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(3, 1, 2, 0));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(1, 0, 3, 2));

			row2 = _mm_add_ps(row2, _mm_mul_ps(tmp1, tmp2));

			// Compute the third multiplication and second subtraction for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(1, 0, 3, 2));

			row3 = _mm_sub_ps(row3, _mm_mul_ps(tmp1, tmp2));

			// Compute the fourth multiplication and second subtraction for the third row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(1, 0, 3, 2));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(1, 0, 3, 2));

			row2 = _mm_sub_ps(row2, _mm_mul_ps(tmp1, tmp2));

			// Compute the fourth multiplication and first addition for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(1, 0, 3, 2));

			row3 = _mm_add_ps(_mm_mul_ps(tmp1, tmp2), row3);

			// Compute the fifth multiplication and second addition for the third row
			tmp1 = _mm_shuffle_ps(cf2, cf2, _MM_SHUFFLE(1, 3, 2, 0));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(0, 1, 2, 3));

			row2 = _mm_add_ps(row2, _mm_mul_ps(tmp1, tmp2));

			// Compute the fifth multiplication and third subtraction for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(0, 1, 2, 3));

			row3 = _mm_sub_ps(row3, _mm_mul_ps(tmp1, tmp2));

			// Compute the sixth multiplication and third subtraction for the third row
			tmp1 = _mm_shuffle_ps(tmp1, tmp1, _MM_SHUFFLE(0, 1, 2, 3));
			tmp2 = _mm_shuffle_ps(mTmp.r[3], mTmp.r[3], _MM_SHUFFLE(0, 1, 2, 3));

			row2 = _mm_sub_ps(row2, _mm_mul_ps(tmp1, tmp2));

			// Compute the sixth multiplication and second addition for the fourth row
			tmp2 = _mm_shuffle_ps(mTmp.r[2], mTmp.r[2], _MM_SHUFFLE(0, 1, 2, 3));

			row3 = _mm_add_ps(row3, _mm_mul_ps(tmp1, tmp2));

			// Compute the determinant
			BVVector det = _mm_mul_ps(row0, mTmp.r[0]);

			// Sum all values and replicate them through all elements
			det = _mm_add_ps(det, _mm_shuffle_ps(det, det, _MM_SHUFFLE(2, 3, 0, 1)));
			det = _mm_add_ps(det, _mm_shuffle_ps(det, det, _MM_SHUFFLE(1, 0, 3, 2)));

			// Get 1/Det
			det = _mm_rcp_ps(det); // _mm_rcp_ps is faster but less precise
			//det = _mm_div_ps(_mm_set1_ps(1.0f), det); // _mm_div_ps is not as fast as _mm_rcp_ps but is more precise

			// Multiply every element by 1/Det
			mTmp.r[0] = _mm_mul_ps(row0, det);
			mTmp.r[1] = _mm_mul_ps(row1, det);
			mTmp.r[2] = _mm_mul_ps(row2, det);
			mTmp.r[3] = _mm_mul_ps(row3, det);

			return mTmp;
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
			BVVector vCosSin = VectorFromFloat2Ptr(cosSin);
			cosSin[1] = -cosSin[1];
			BVVector vMinusSinCos = VectorFromFloat2Ptr(cosSin);

			BVMatrix r;
			r.r[0] = VectorSet(1.0f, 0.0f, 0.0f, 0.0f);
			r.r[1] = _mm_shuffle_ps(vCosSin, vCosSin, _MM_SHUFFLE(3, 1, 0, 2));
			r.r[2] = _mm_shuffle_ps(vMinusSinCos, vMinusSinCos, _MM_SHUFFLE(3, 0, 1, 2));
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationY(const float angle)
		{
			float cosSin[] = { cosf(angle), sinf(angle) };
			BVVector vCosSin = VectorFromFloat2Ptr(cosSin);
			cosSin[1] = -cosSin[1];
			BVVector vMinusSinCos = VectorFromFloat2Ptr(cosSin);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(vMinusSinCos, vMinusSinCos, _MM_SHUFFLE(3, 1, 2, 0));
			r.r[1] = VectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			r.r[2] = _mm_shuffle_ps(vCosSin, vCosSin, _MM_SHUFFLE(3, 0, 2, 1));
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
			r.r[0] = vCosSin;
			r.r[1] = _mm_shuffle_ps(vMinusSinCos, vMinusSinCos, _MM_SHUFFLE(3, 2, 0, 1));
			r.r[2] = VectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			r.r[3] = VectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			return r;
		}

		inline BVMatrix MatrixRotationAxis(CBVVector axis, const float angle)
		{
			float sin = sinf(angle);
			float cos = cosf(angle);

			BVVector vSin = _mm_set1_ps(sin);
			BVVector vCos = _mm_set_ps(0.0f, cos, cos, cos);
			BVVector vOneMinusCos = _mm_set1_ps(1.0f - cos);

			// x * (1 - cos), y * (1 - cos), z * (1 - cos)
			BVVector d = _mm_mul_ps(axis, vOneMinusCos);

			BVVector t1 = d;

			// x^2 * (1 - cos), y^2 * (1 - cos), z^2 * (1 - cos)
			d = _mm_mul_ps(d, axis);
			// cos + x^2 * (1 - cos), cos + y^2 * (1 - cos), cos + z^2 * (1 - cos)
			d = _mm_add_ps(d, vCos);

			// x * (sin), y * (sin), z * (sin)
			BVVector t0 = _mm_mul_ps(axis, vSin);

			// z * (1 - cos), x * (1 - cos), y * (1 - cos)
			t1 = _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(3, 1, 0, 2));
			// y * z * (1 - cos), z * x * (1 - cos), x * y * (1 - cos)
			t1 = _mm_mul_ps(t1, _mm_shuffle_ps(axis, axis, _MM_SHUFFLE(3, 0, 2, 1)));
			// y * z * (1 - cos) - x * (sin), z * x * (1 - cos) - y * (sin), x * y * (1 - cos) - z * (sin)
			BVVector t2 = _mm_sub_ps(t1, t0);
			// y * z * (1 - cos) + x * (sin), z * x * (1 - cos) + y * (sin), x * y * (1 - cos) + z * (sin)
			t1 = _mm_add_ps(t1, t0);

			// y * z * (1 - cos) + x * (sin), x * y * (1 - cos) + z * (sin), z * x * (1 - cos) - y * (sin), x * y * (1 - cos) - z * (sin)
			BVVector g0 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(2, 1, 2, 0));
			// y * z * (1 - cos) + x * (sin), 0, y * z * (1 - cos) - x * (sin), 0
			BVVector g1 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(3, 0, 3, 1));

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(d, g0, _MM_SHUFFLE(2, 1, 3, 0));
			r.r[0] = _mm_shuffle_ps(r.r[0], r.r[0], _MM_SHUFFLE(1, 3, 2, 0));

			r.r[1] = _mm_shuffle_ps(d, g0, _MM_SHUFFLE(0, 3, 3, 1));
			r.r[1] = _mm_shuffle_ps(r.r[1], r.r[1], _MM_SHUFFLE(1, 3, 0, 2));

			r.r[2] = _mm_shuffle_ps(d, g1, _MM_SHUFFLE(2, 0, 3, 2));
			r.r[2] = _mm_shuffle_ps(r.r[2], r.r[2], _MM_SHUFFLE(1, 0, 3, 2));

			r.r[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

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

			BVVector v0 = _mm_set_ps(0.0f, -nearZ * b, a, a / aspectRatio);
			BVVector v1 = _mm_set_ps(0.0f, 0.0f, 1.0f, b);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1, 0, 3, 3));
			r.r[3] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));

			return r;
		}

		inline BVMatrix MatrixPerspectiveRH_DX(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ / (farZ - nearZ);

			BVVector v0 = _mm_set_ps(0.0f, nearZ * b, a, a / aspectRatio);
			BVVector v1 = _mm_set_ps(0.0f, 0.0f, -1.0f, b);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1, 0, 3, 3));
			r.r[3] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));

			return r;
		}

		inline BVMatrix MatrixPerspectiveLH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ - nearZ;

			BVVector v0 = _mm_set_ps(0.0f, (2.0f * farZ * nearZ) / b, a, a / aspectRatio);
			BVVector v1 = _mm_set_ps(0.0f, 0.0f, 1.0f, -(farZ + nearZ) / b);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1, 0, 3, 3));
			r.r[3] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));

			return r;
		}

		inline BVMatrix MatrixPerspectiveRH_GL(const float nearZ, const float farZ, const float aspectRatio, const float fovY)
		{
			float a = 1.0f / tanf(fovY * 0.5f);
			float b = farZ - nearZ;

			BVVector v0 = _mm_set_ps(0.0f, (-2.0f * farZ * nearZ) / b, a, a / aspectRatio);
			BVVector v1 = _mm_set_ps(0.0f, 0.0f, -1.0f, -(farZ + nearZ) / b);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(1, 0, 3, 3));
			r.r[3] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));

			return r;
		}

		inline BVMatrix MatrixOrthographicLH_DX(const float width, const float height, const float nearZ, const float farZ)
		{
			float twoOverWidth = 2.0f / (width);
			float twoOverHeight = 2.0f / (height);
			float oneOverFMinusN = 1.0f / (farZ - nearZ);

			BVVector v0 = VectorSet(twoOverWidth, twoOverHeight, oneOverFMinusN, 0.0f);
			BVVector v1 = VectorSet(0.0f, 0.0f,	-oneOverFMinusN * nearZ, 1.0f);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));
			r.r[3] = v1;

			return r;
		}

		inline BVMatrix MatrixOrthographicOffCenterLH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverFMinusN = 1.0f / (farZ - nearZ);

			BVVector v0 = VectorSet(oneOverRMinusL + oneOverRMinusL, oneOverTMinusB + oneOverTMinusB, oneOverFMinusN, 0.0f);
			BVVector v1 = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB,
				-oneOverFMinusN * nearZ, 1.0f);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));
			r.r[3] = v1;

			return r;
		}

		inline BVMatrix MatrixOrthographicOffCenterRH_DX(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverNMinusF = 1.0f / (nearZ - farZ);

			BVVector v0 = VectorSet(oneOverRMinusL + oneOverRMinusL, oneOverTMinusB + oneOverTMinusB, oneOverNMinusF, 0.0f);
			BVVector v1 = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB,
				oneOverNMinusF * nearZ, 1.0f);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));
			r.r[3] = v1;

			return r;
		}

		inline BVMatrix MatrixOrthographicOffCenterLH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverNMinusF = 1.0f / (nearZ - farZ);

			BVVector v0 = VectorSet(oneOverRMinusL + oneOverRMinusL, oneOverTMinusB + oneOverTMinusB, -2.0f * oneOverNMinusF, 0.0f);
			BVVector v1 = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB,
				(nearZ + farZ) * oneOverNMinusF, 1.0f);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));
			r.r[3] = v1;

			return r;
		}

		inline BVMatrix MatrixOrthographicOffCenterRH_GL(const float right, const float left, const float top, const float bottom, const float
			nearZ, const float farZ)
		{
			float oneOverRMinusL = 1.0f / (right - left);
			float oneOverTMinusB = 1.0f / (top - bottom);
			float oneOverFMinusN = 1.0f / (farZ - nearZ);

			BVVector v0 = VectorSet(oneOverRMinusL + oneOverRMinusL, oneOverTMinusB + oneOverTMinusB, -2.0f * oneOverFMinusN, 0.0f);
			BVVector v1 = VectorSet(-(left + right) * oneOverRMinusL, -(bottom + top) * oneOverTMinusB,
				-(nearZ + farZ) * oneOverFMinusN, 1.0f);

			BVMatrix r;
			r.r[0] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 3, 0));
			r.r[1] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 3, 1, 3));
			r.r[2] = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 2, 3, 3));
			r.r[3] = v1;

			return r;
		}
	}
}

#endif