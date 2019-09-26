#ifndef SAFEHEAPALLOCATOR_H
#define SAFEHEAPALLOCATOR_H

#include "HeapAllocator.h"
#include "MemoryUtils.h"

namespace BVMemory
{
	class SafeHeapAllocator : public HeapAllocator
	{
	public:
		SafeHeapAllocator();
		SafeHeapAllocator(const size_t size);
		SafeHeapAllocator(void * const pStart, void * const pEnd);
		~SafeHeapAllocator();

		void *AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0) override;
		void FreeA(void * const pAddress) override;

		void *AllocateU(const size_t size) override;
		void FreeU(void * const pAddress) override;

		void Clear() override;

	private:
		SafeHeapAllocator(const SafeHeapAllocator & other) = delete;
		SafeHeapAllocator(SafeHeapAllocator && other) = delete;
		SafeHeapAllocator & operator = (const SafeHeapAllocator & other) = delete;
		SafeHeapAllocator & operator = (SafeHeapAllocator && other) = delete;

	protected:
		CRITICAL_SECTION m_CS;
	};
}

#endif