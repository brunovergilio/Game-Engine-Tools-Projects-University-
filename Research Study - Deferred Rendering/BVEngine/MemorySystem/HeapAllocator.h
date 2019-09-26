#ifndef HEAPALLOCATOR_H
#define HEAPALLOCATOR_H

#include "Allocator.h"

namespace BVMemory
{
	class HeapAllocator : public Allocator
	{
	public:
		HeapAllocator();
		HeapAllocator(const size_t size);
		HeapAllocator(void * const pStart, void * const pEnd);
		virtual ~HeapAllocator();

		void Create(const size_t size);
		void Assign(void * const pStart, void * const pEnd);

		virtual void *AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0);
		virtual void FreeA(void * const pAddress);

		virtual void *AllocateU(const size_t size);
		virtual void FreeU(void * const pAddress);

		virtual void Clear();

	private:
		struct HeapHeader
		{
			HeapHeader *m_pNext;
			HeapHeader *m_pPrev;
			size_t m_Size;
			bool m_Used;

			HeapHeader() : m_pNext(nullptr), m_pPrev(nullptr), m_Size(0), m_Used(false) {}
		};
		static const size_t MIN_HEAP_BLOCK_SIZE = (sizeof(HeapHeader) + sizeof(size_t) + 1);

		void CreateFreeHeader(void * const pAddress, const size_t size);
		HeapHeader *CreateUsedHeader(const size_t size);
		void RemoveHeader(HeapHeader * const pHeader);

		HeapAllocator(const HeapAllocator & other) = delete;
		HeapAllocator(HeapAllocator && other) = delete;
		HeapAllocator & operator = (const HeapAllocator & other) = delete;
		HeapAllocator & operator = (HeapAllocator && other) = delete;

	private:
		char *m_pStart;
		char *m_pEnd;
		HeapHeader *m_pFreeHeapList;
		HeapHeader *m_pUsedHeapList;
	};
}
#endif