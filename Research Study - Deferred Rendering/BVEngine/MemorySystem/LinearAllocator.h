#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include "Allocator.h"

namespace BVMemory
{
	class LinearAllocator : public Allocator
	{
	public:
		LinearAllocator();
		LinearAllocator(const size_t size);
		LinearAllocator(void * const pStart, void * const pEnd);
		~LinearAllocator();

		void Create(const size_t size);
		void Assign(void * const pStart, void * const pEnd);

		void *AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0) override;
		// Doesn't do anything
		inline void FreeA(void * const) override { }

		void *AllocateU(const size_t size) override;
		// Doesn't do anything
		inline void FreeU(void * const) override { };

		void Clear() override;

	private:
		LinearAllocator(const LinearAllocator & other) = delete;
		LinearAllocator(LinearAllocator && other) = delete;
		LinearAllocator & operator = (const LinearAllocator & other) = delete;
		LinearAllocator & operator = (LinearAllocator && other) = delete;

	private:
		char *m_pStart;
		char *m_pEnd;
		char *m_pCurrent;
	};
}
#endif