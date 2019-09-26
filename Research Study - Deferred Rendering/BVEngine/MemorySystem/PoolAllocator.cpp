#include "PoolAllocator.h"

#include "MemoryUtils.h"

namespace BVMemory
{
	PoolAllocator::PoolAllocator()
		: m_pNext(nullptr)
	{
	}


	PoolAllocator::PoolAllocator(const size_t size, const size_t alignment)
		: m_pNext(nullptr)
	{
		Create(size, alignment);
	}


	PoolAllocator::PoolAllocator(void * const pStart, void * const pEnd, const size_t size, const size_t alignment)
		: m_pNext(nullptr)
	{
		InitializeList(pStart, pEnd, size, alignment);
	}


	void PoolAllocator::InitializeList(void *pStart, void *pEnd, const size_t size, const size_t alignment)
	{
		BV_ASSERT(pStart != nullptr && pEnd != nullptr, "PoolAllocator::InitializeList -> Memory parameters must not be null");
		BV_ASSERT(size > 0, "PoolAllocator::InitializeList -> The size parameter cannot be 0");

		MemoryAddress startAddress(pStart);
		MemoryAddress endAddress(pEnd);

		startAddress.asVoidPtr = Align(startAddress.asVoidPtr, alignment);

		BV_ASSERT(endAddress.asSize_t - startAddress.asSize_t > size, "PoolAllocator::InitializeList -> Memory block is too small");

		BV_ASSERT(endAddress.asSize_t > startAddress.asSize_t, "PoolAllocator::InitializeList -> Aligned memory block is past bounds");
		BV_ASSERT(endAddress.asSize_t - startAddress.asSize_t > sizeof(void *), "PoolAllocator::InitializeList -> Aligned memory block is too small");
		BV_ASSERT(endAddress.asSize_t - startAddress.asSize_t > size, "PoolAllocator::InitializeList -> Aligned memory block is too small");

		// Use union to make things easier
		union
		{
			PoolAllocator *asListPtr;
			char *asCharPtr;
		} freeListAddr;
		freeListAddr.asCharPtr = startAddress.asCharPtr;

		// Assign the first address to the m_pNext pointer, and then increment the value
		m_pNext = freeListAddr.asListPtr;
		freeListAddr.asCharPtr += size;

		// Do it for the other elements (keep going as long as there's space)
		PoolAllocator *pNextElement = m_pNext;
		while(freeListAddr.asCharPtr < endAddress.asCharPtr)
		{
			pNextElement->m_pNext = freeListAddr.asListPtr;
			pNextElement = freeListAddr.asListPtr;
			freeListAddr.asCharPtr += size;
		}

		pNextElement->m_pNext = nullptr;
	}


	PoolAllocator::~PoolAllocator()
	{
		m_pNext = nullptr;
	}


	void PoolAllocator::Create(const size_t size, const size_t alignment)
	{
		m_MemoryArea.Create(size);

		InitializeList(m_MemoryArea.GetStart(), m_MemoryArea.GetEnd(), size, alignment);
	}


	void PoolAllocator::Assign(void * const pStart, void * const pEnd, const size_t size, const size_t alignment)
	{
		InitializeList(pStart, pEnd, size, alignment);
	}


	void *PoolAllocator::AllocateA(const size_t, const size_t, const size_t)
	{
		if (!m_pNext)
		{
			return nullptr;
		}

		void *pMem = m_pNext;
		m_pNext = m_pNext->m_pNext;

		return pMem;
	}


	void PoolAllocator::FreeA(void * const pAddress)
	{
		PoolAllocator *pAllocator = reinterpret_cast<PoolAllocator *>(pAddress);
		pAllocator->m_pNext = m_pNext;
		m_pNext = pAllocator;
	}


	void *PoolAllocator::AllocateU(const size_t size)
	{
		return AllocateA();
	}


	void PoolAllocator::FreeU(void * const pAddress)
	{
		FreeA(pAddress);
	}
}