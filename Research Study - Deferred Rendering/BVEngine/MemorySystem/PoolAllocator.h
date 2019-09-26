#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include "Allocator.h"

namespace BVMemory
{
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator();
		PoolAllocator(const size_t size, const size_t alignment);
		PoolAllocator(void * const pStart, void * const pEnd, const size_t size, const size_t alignment);
		~PoolAllocator();

		void Create(const size_t size, const size_t alignment);
		void Assign(void * const pStart, void * const pEnd, const size_t size, const size_t alignment);

		// These 3 parameters are ignored
		void *AllocateA(const size_t size = 0, const size_t alignment = 0, const size_t bytesToAlignAfter = 0) override;
		void FreeA(void * const pAddress) override;

		void *AllocateU(const size_t size = 0) override;
		void FreeU(void * const pAddress) override;

		// Doesn't do anything
		inline void Clear() override {};

	private:
		void InitializeList(void *pStart, void *pEnd, const size_t size, const size_t alignment);

		PoolAllocator(const PoolAllocator & other) = delete;
		PoolAllocator(PoolAllocator && other) = delete;
		PoolAllocator & operator = (const PoolAllocator & other) = delete;
		PoolAllocator & operator = (PoolAllocator && other) = delete;

	private:
		PoolAllocator *m_pNext;
	};
}
#endif