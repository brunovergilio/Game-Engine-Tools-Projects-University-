#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

#include "Allocator.h"

namespace BVMemory
{
	class StackAllocator : public Allocator
	{
	public:
		StackAllocator();
		StackAllocator(const size_t size);
		StackAllocator(void * const pStart, void * const pEnd);
		~StackAllocator();

		void Create(const size_t size);
		void Assign(void * const pStart, void * const pEnd);

		void *AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0) override;
		void FreeA(void * const pAddress) override;

		void *AllocateU(const size_t size) override;
		void FreeU(void * const pAddress) override;

		void Clear() override;

	private:
		StackAllocator(const StackAllocator & other) = delete;
		StackAllocator(StackAllocator && other) = delete;
		StackAllocator & operator = (const StackAllocator & other) = delete;
		StackAllocator & operator = (StackAllocator && other) = delete;

	private:
		char *m_pStart;
		char *m_pCurrent;
		char *m_pEnd;
		unsigned int m_LastAllocation;
	};
}
#endif