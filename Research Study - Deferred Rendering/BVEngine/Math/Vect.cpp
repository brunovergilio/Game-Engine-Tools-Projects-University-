//#include "Vect.h"
//
//namespace BVMath
//{
//	// =================================
//	// Big 6 (constructors / destructor)
//	// =================================
//
//	Vect::Vect()
//		: vector()
//	{
//	}
//
//
//	Vect::~Vect()
//	{
//	}
//
//
//	Vect::Vect(const Vect & other)
//		: vector(other.vector)
//	{
//	}
//
//
//	Vect::Vect(Vect && other)
//		: vector(other.vector)
//	{
//		other.x = other.y = other.z = other.w = 0.0f;
//	}
//
//
//	Vect & Vect::operator = (const Vect & other)
//	{
//		if (this != &other)
//		{
//			vector = other.vector;
//		}
//
//		return *this;
//	}
//
//
//	Vect & Vect::operator = (Vect && other)
//	{
//		if (this != &other)
//		{
//			vector = other.vector;
//			other.x = other.y = other.z = other.w = 0.0f;
//		}
//
//		return *this;
//	}
//
//
//	// =================================
//	// Specialized constructors
//	// =================================
//
//	Vect::Vect(const float x, const float y, const float z, const float w)
//		: x(x), y(y), z(z), w(w)
//	{
//	}
//
//	Vect::Vect(BVVector v)
//		: vector(v)
//	{
//	}
//
//
//	// =================================
//	// Accessors / modifiers
//	// =================================
//
//	const float Vect::operator [] (const size_t index) const
//	{
//		return v[index];
//	}
//
//
//	float & Vect::operator [] (const size_t index)
//	{
//		return v[index];
//	}
//
//	void Vect::Set(const float x, const float y, const float z, const float w)
//	{
//		vector = VectorSet(x, y, z, w);
//	}
//
//	void Vect::Set(const Vect & v)
//	{
//		vector = v.vector;
//	}
//
//	void Vect::Set(CBVVector v)
//	{
//		vector = v;
//	}
//
//	// BVVector cast
//	Vect::operator BVVector () const
//	{
//		return vector;
//	}
//
//	// =================================
//	// Basic Operations
//	// =================================
//
//	const bool Vect::IsZero(const float epsilon) const
//	{
//		return VectorIsZero(vector, epsilon);
//	}
//
//	const bool Vect::IsEqual(const Vect &v, const float epsilon) const
//	{
//		return VectorIsEqual(vector, v, epsilon);
//	}
//}