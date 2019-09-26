#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "MemoryArea.h"

namespace BVMemory
{
	class Allocator
	{
	public:
		Allocator();
		virtual ~Allocator();

		virtual void *AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0) = 0;
		virtual void FreeA(void * const pAddress) = 0;

		virtual void *AllocateU(const size_t size) = 0;
		virtual void FreeU(void * const pAddress) = 0;

		virtual void Clear() = 0;

	private:
		Allocator(const Allocator & other) = delete;
		Allocator(Allocator && other) = delete;
		Allocator & operator = (const Allocator & other) = delete;
		Allocator & operator = (Allocator && other) = delete;

	protected:
		MemoryArea m_MemoryArea;
	};
}
#endif