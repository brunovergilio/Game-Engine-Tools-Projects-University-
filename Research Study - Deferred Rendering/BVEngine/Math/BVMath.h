#ifndef BVMATH_H
#define BVMATH_H

// Includes and versions, for future reference
//
//<mmintrin.h>  MMX
//<xmmintrin.h> SSE
//<emmintrin.h> SSE2
//<pmmintrin.h> SSE3
//<tmmintrin.h> SSSE3
//<smmintrin.h> SSE4.1
//<nmmintrin.h> SSE4.2
//<ammintrin.h> SSE4A
//<wmmintrin.h> AES
//<immintrin.h> AVX
//<zmmintrin.h> AVX512

#include <nmmintrin.h>
#include <cmath>

// Comment out for regular FPU instruction set
#define BV_USE_SIMD

// Default floating point error threshold
#define BV_EPSILON 0.0001f

// Trigonometric values
#define PI 3.1415926535f
#define TWO_TIMES_PI 6.283059307f
#define ONE_OVER_PI 0.3183098861f
#define ONE_OVER_TWO_TIMES_PI 0.1591581347f
#define PI_DIV_180 0.0174532925f
#define ONE_OVER_PI_DIV_180 57.2969285199f
#define PI_SQR 9.8696044005f
#define FOUR_OVER_PI 1.2732395447f
#define FOUR_OVER_PI_SQR 0.4052847345f
#define PI_OVER_TWO 1.5707648267f
#define PI_OVER_FOUR 0.7853981633f

namespace BVMath
{
	// Containers
	struct BVFloat4
	{
		float x, y, z, w;
		BVFloat4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		BVFloat4(const float x, const float y, const float z, const float w = 0.0f)
			: x(x), y(y), z(z), w(w) {}
	};

	struct BVFloat3
	{
		float x, y, z;
		BVFloat3() : x(0.0f), y(0.0f), z(0.0f) {}
		BVFloat3(const float x, const float y, const float z)
			: x(x), y(y), z(z) {}
	};

	struct BVFloat2
	{
		float x, y;
		BVFloat2() : x(0.0f), y(0.0f) {}
		BVFloat2(const float x, const float y)
			: x(x), y(y) {}
	};

	struct BVFloat33
	{
		BVFloat3 r[3];

		BVFloat33()	{}
		BVFloat33(const BVFloat3 & r0, const BVFloat3 & r1, const BVFloat3 & r2)
		{
			r[0] = r0;
			r[1] = r1;
			r[2] = r2;
		}
		BVFloat33(const float a00, const float a01, const float a02,
			const float a10, const float a11, const float a12,
			const float a20, const float a21, const float a22)
		{
			r[0] = BVFloat3(a00, a01, a02);
			r[1] = BVFloat3(a10, a11, a12);
			r[2] = BVFloat3(a20, a21, a22);
		}
		BVFloat33(const BVFloat33 & m)
		{
			r[0] = m.r[0];
			r[1] = m.r[1];
			r[2] = m.r[2];
		}
	};

	struct BVFloat44
	{
		BVFloat4 r[4];

		BVFloat44() {}
		BVFloat44(const BVFloat4 & r0, const BVFloat4 & r1, const BVFloat4 & r2, const BVFloat4 & r3)
		{
			r[0] = r0;
			r[1] = r1;
			r[2] = r2;
			r[3] = r3;
		}
		BVFloat44(const float a00, const float a01, const float a02, const float a03,
			const float a10, const float a11, const float a12, const float a13,
			const float a20, const float a21, const float a22, const float a23,
			const float a30, const float a31, const float a32, const float a33)
		{
			r[0] = BVFloat4(a00, a01, a02, a03);
			r[1] = BVFloat4(a10, a11, a12, a13);
			r[2] = BVFloat4(a20, a21, a22, a23);
			r[3] = BVFloat4(a30, a31, a32, a33);
		}
		BVFloat44(const BVFloat44 & m)
		{
			r[0] = m.r[0];
			r[1] = m.r[1];
			r[2] = m.r[2];
			r[3] = m.r[3];
		}
	};

	// SIMD structures
	namespace SIMD
	{
		typedef __m128 BVVector;
		typedef const BVVector CBVVector;
		typedef const BVVector & CRBVVector;
		typedef BVVector & RBVVector;

		__declspec(align(16)) struct BVMatrix
		{
			BVVector r[4];

			BVMatrix() {}
			BVMatrix(CBVVector r0, CBVVector r1, CBVVector r2, CRBVVector r3)
			{
				r[0] = r0;
				r[1] = r1;
				r[2] = r2;
				r[3] = r3;
			}
			BVMatrix(const float a00, const float a01, const float a02, const float a03,
				const float a10, const float a11, const float a12, const float a13,
				const float a20, const float a21, const float a22, const float a23,
				const float a30, const float a31, const float a32, const float a33)
			{
				r[0] = _mm_set_ps(a03, a02, a01, a00);
				r[1] = _mm_set_ps(a13, a12, a11, a10);
				r[2] = _mm_set_ps(a23, a22, a21, a20);
				r[3] = _mm_set_ps(a33, a32, a31, a30);
			}
			BVMatrix(const BVMatrix & m)
			{
				r[0] = m.r[0];
				r[1] = m.r[1];
				r[2] = m.r[2];
				r[3] = m.r[3];
			}
		};

		typedef const BVMatrix CBVMatrix;
		typedef const BVMatrix & CRBVMatrix;
		typedef BVMatrix & RBVMatrix;

		typedef BVVector BVQuat;
		typedef CBVVector CBVQuat;
		typedef CRBVVector CRBVQuat;
		typedef RBVVector RBVQuat;
	}

	// FPU structures
	namespace FPU
	{
		typedef BVFloat4 BVVector;
		typedef const BVFloat4 CTBVVector;
		typedef const BVFloat4 & CBVVector;
		typedef const BVFloat4 & CRBVVector;
		typedef BVFloat4 & RBVVector;

		typedef BVFloat44 BVMatrix;
		typedef const BVFloat44 CTBVMatrix;
		typedef const BVFloat44 & CBVMatrix;
		typedef const BVFloat44 & CRBVMatrix;
		typedef BVFloat44 & RBVMatrix;

		typedef BVVector BVQuat;
		typedef CBVVector CBVQuat;
		typedef CRBVVector CRBVQuat;
		typedef RBVVector RBVQuat;
	}

	// Utility functions
	//float Sin(const float rad);
	//float Cos(const float rad);
	//float Tan(const float val);
	//float ASin(const float val);
	//float ACos(const float val);
	//float ATan(const float val);
	float AngleToRadians(const float angle);
	float RadiansToAngle(const float rad);

	//inline float Sin(const float rad)
	//{
	//	float x = rad;
	//	if (rad < 0.0f || rad > TWO_TIMES_PI)
	//	{
	//		x = rad * ONE_OVER_TWO_TIMES_PI;
	//		x -= (float)((int)x);
	//		x *= TWO_TIMES_PI;
	//	}

	//	float sign = 1.0f;
	//	if (x > PI)
	//	{

	//	}

	//	return x;
	//}

	//inline float Cos(const float rad)
	//{
	//	return 0.0f;
	//}

	//inline float ASin(const float val)
	//{
	//	return 0.0f;
	//}

	//inline float ACos(const float val)
	//{
	//	return 0.0f;
	//}

	inline float AngleToRadians(const float angle)
	{
		return angle * PI_DIV_180;
	}

	inline float RadiansToAngle(const float rad)
	{
		return rad * ONE_OVER_PI_DIV_180;
	}
}

#endif