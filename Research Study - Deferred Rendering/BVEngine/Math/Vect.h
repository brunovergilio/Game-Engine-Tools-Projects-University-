//#ifndef VECT_H
//#define VECT_H
//
//// nameless union
//#pragma warning(disable : 4201)
//
//#include "BVMath.h"
//
//#ifdef BV_USE_SIMD
//#include "BVVector_SIMD.h"
//using namespace BVMath::SIMD;
//#else
//#include "BVVector_FPU.h"
//using namespace BVMath::FPU;
//#endif
//
//namespace BVMath
//{
//	class Vect
//	{
//	public:
//		// =================================
//		// Big 6 (constructors / destructor)
//		// =================================
//
//		Vect();
//		~Vect();
//		Vect(const Vect & other);
//		Vect(Vect && other);
//		Vect & operator = (const Vect & other);
//		Vect & operator = (Vect && other);
//
//		// =================================
//		// Specialized constructors
//		// =================================
//		
//		Vect(const float x, const float y, const float z, const float w = 0.0f);
//		Vect(BVVector v);
//
//		// =================================
//		// Accessors / modifiers
//		// =================================
//
//		const float operator [] (const size_t index) const;
//		float & operator [] (const size_t index);
//		
//		void Set(const float x, const float y, const float z, const float w = 0.0f);
//		void Set(const Vect & v);
//		void Set(CBVVector v);
//
//		// BVVector cast
//		operator BVVector () const;
//
//		// =================================
//		// Basic Operations
//		// =================================
//
//		const bool IsZero(const float epsilon = BV_EPSILON) const;
//		const bool IsEqual(const Vect &v, const float epsilon = BV_EPSILON) const;
//
//		Vect operator + (const Vect & other) const;
//		Vect & operator += (const Vect & other);
//		Vect operator - (const Vect & other) const;
//		Vect & operator -= (const Vect & other);
//
//		Vect operator + ();
//		Vect operator - ();
//
//		Vect operator * (const float & val);
//		friend Vect operator * (const float & val, const Vect & other);
//		Vect & operator *= (const float & val);
//
//		float Dot(const Vect & other) const;
//		Vect Cross(const Vect & other) const;
//		void Normalize();
//		Vect GetNormalized() const;
//		float Length() const;
//		float LengthSqr() const;
//		float GetAngle(const Vect & other) const;
//
//		//friend Vect operator * (const Vect & v, const Matrix & m);
//		//Vect & operator *= (const Matrix & m);
//
//	public:
//		union
//		{
//			float v[4];
//			struct
//			{
//				float x, y, z, w;
//			};
//			struct
//			{
//				BVVector vector;
//			};
//			struct
//			{
//				BVFloat4 float4;
//			};
//			struct
//			{
//				BVFloat3 float3;
//			};
//			struct
//			{
//				BVFloat2 float2;
//			};
//		};
//	};
//}
//#endif