#include "HeapAllocator.h"

#include "MemoryUtils.h"

namespace BVMemory
{
	HeapAllocator::HeapAllocator()
		: m_pStart(nullptr), m_pEnd(nullptr), m_pFreeHeapList(nullptr), m_pUsedHeapList(nullptr)
	{
	}


	HeapAllocator::HeapAllocator(const size_t size)
		: m_pStart(nullptr), m_pEnd(nullptr), m_pFreeHeapList(nullptr), m_pUsedHeapList(nullptr)
	{
		Create(size);
	}


	HeapAllocator::HeapAllocator(void *const pStart, void * const pEnd)
		: m_pStart(reinterpret_cast<char *>(pStart)), m_pEnd(reinterpret_cast<char *>(pEnd)), m_pFreeHeapList(nullptr), m_pUsedHeapList(nullptr)
	{
		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "HeapAllocator::HeapAllocator -> Memory parameters must not be null");
		size_t size = reinterpret_cast<size_t>(m_pEnd) - reinterpret_cast<size_t>(m_pStart);
		BV_ASSERT(size >= MIN_HEAP_BLOCK_SIZE, "HeapAllocator::HeapAllocator -> Memory space is insufficient for a free heap block");

		CreateFreeHeader(m_pStart, size - sizeof(HeapHeader) - sizeof(size_t));
	}


	HeapAllocator::~HeapAllocator()
	{
		m_pStart = nullptr;
		m_pEnd = nullptr;
		m_pFreeHeapList = nullptr;
		m_pUsedHeapList = nullptr;
	}


	void HeapAllocator::Create(const size_t size)
	{
		m_MemoryArea.Create(size);
		m_pStart = m_MemoryArea.GetStart();
		m_pEnd = m_MemoryArea.GetEnd();

		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "HeapAllocator::Create -> Memory parameters must not be null");
		size_t blockSize = reinterpret_cast<size_t>(m_pEnd) - reinterpret_cast<size_t>(m_pStart);
		BV_ASSERT(blockSize >= MIN_HEAP_BLOCK_SIZE, "HeapAllocator::Create -> Memory space is insufficient for a free heap block");

		CreateFreeHeader(m_pStart,
			reinterpret_cast<size_t>(m_MemoryArea.GetEnd()) - reinterpret_cast<size_t>(m_MemoryArea.GetStart()) - sizeof(HeapHeader) - sizeof(size_t));
	}


	void HeapAllocator::Assign(void * const pStart, void * const pEnd)
	{
		m_pStart = reinterpret_cast<char *>(pStart);
		m_pEnd = reinterpret_cast<char *>(pEnd);

		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "HeapAllocator::Assign -> Memory parameters must not be null");
		size_t size = reinterpret_cast<size_t>(m_pEnd) - reinterpret_cast<size_t>(m_pStart);
		BV_ASSERT(size >= MIN_HEAP_BLOCK_SIZE, "HeapAllocator::Assign -> Memory space is insufficient for a free heap block");

		CreateFreeHeader(m_pStart, size - sizeof(HeapHeader) - sizeof(size_t));
	}


	void *HeapAllocator::AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter)
	{
		HeapHeader *pNewHeader = CreateUsedHeader(size + alignment + bytesToAlignAfter);
		if (!pNewHeader)
		{
			return nullptr;
		}

		MemoryAddress freeAddress(pNewHeader);
		freeAddress.asCharPtr += sizeof(HeapHeader);
		return Align(freeAddress.asVoidPtr, alignment, bytesToAlignAfter);
	}


	void HeapAllocator::FreeA(void * const pAddress)
	{
		// Get the address
		MemoryAddress currAddress(pAddress);
		// Move it back by the number of aligned bytes (stored in one byte before the address) and by the size of the HeapHeader
		HeapHeader *pUsedHeader = reinterpret_cast<HeapHeader *>(currAddress.asCharPtr - currAddress.asCharPtr[-1] - sizeof(HeapHeader));

		RemoveHeader(pUsedHeader);

		CreateFreeHeader(pUsedHeader, pUsedHeader->m_Size);
	}


	void *HeapAllocator::AllocateU(const size_t size)
	{
		HeapHeader *pNewHeader = CreateUsedHeader(size);
		if (!pNewHeader)
		{
			return nullptr;
		}

		MemoryAddress freeAddress(pNewHeader);
		freeAddress.asCharPtr += sizeof(HeapHeader);
		return freeAddress.asVoidPtr;
	}


	void HeapAllocator::FreeU(void * const pAddress)
	{
		// Get the address
		MemoryAddress currAddress(pAddress);
		HeapHeader *pUsedHeader = reinterpret_cast<HeapHeader *>(currAddress.asCharPtr - sizeof(HeapHeader));

		RemoveHeader(pUsedHeader);

		CreateFreeHeader(pUsedHeader, pUsedHeader->m_Size);
	}


	void HeapAllocator::Clear()
	{
		HeapHeader *pWalker = m_pUsedHeapList;
		HeapHeader *pUsedHeader = nullptr;
		while (pWalker)
		{
			pUsedHeader = pWalker;
			pWalker = pWalker->m_pNext;

			RemoveHeader(pUsedHeader);
			CreateFreeHeader(pUsedHeader, pUsedHeader->m_Size);
		}
	}


	void HeapAllocator::CreateFreeHeader(void * const pAddress, const size_t size)
	{
		HeapHeader *pFreeHeader = reinterpret_cast<HeapHeader *>(pAddress);
		pFreeHeader->m_Size = size;
		pFreeHeader->m_Used = false;
		pFreeHeader->m_pNext = m_pFreeHeapList;
		
		if (m_pFreeHeapList)
		{
			m_pFreeHeapList->m_pPrev = pFreeHeader;
		}
		m_pFreeHeapList = pFreeHeader;
		m_pFreeHeapList->m_pPrev = nullptr;

		MemoryAddress nextPointer(pAddress);
		nextPointer.asCharPtr += sizeof(HeapHeader) + pFreeHeader->m_Size;
		*nextPointer.asSize_tPtr = reinterpret_cast<size_t>(pAddress);
		nextPointer.asSize_tPtr++;

		// Merge

		// Merge Front
		size_t heapSize = 0;
		if (nextPointer.asCharPtr < m_pEnd)
		{
			HeapHeader *pNextHeader = reinterpret_cast<HeapHeader *>(nextPointer.asVoidPtr);
			if (!pNextHeader->m_Used)
			{
				RemoveHeader(pNextHeader);
				heapSize = sizeof(HeapHeader) + pNextHeader->m_Size;
				//nextPointer = pNextHeader;
				nextPointer.asCharPtr += heapSize;
				*nextPointer.asSize_tPtr = reinterpret_cast<size_t>(pAddress);
				nextPointer.asSize_tPtr++;

				pFreeHeader->m_Size += heapSize + sizeof(size_t);
			}
		}

		// Merge Back
		if (reinterpret_cast<char *>(pAddress) > m_pStart)
		{
			nextPointer.asVoidPtr = pFreeHeader;
			HeapHeader *pPrevHeader = reinterpret_cast<HeapHeader *>(nextPointer.asSize_tPtr[-1]);
			if (!pPrevHeader->m_Used)
			{
				RemoveHeader(pFreeHeader);
				heapSize = sizeof(HeapHeader) + pFreeHeader->m_Size;
				pPrevHeader->m_Size += heapSize + sizeof(size_t);

				//nextPointer.asSize_tPtr++;
				nextPointer.asCharPtr += sizeof(HeapHeader) + pFreeHeader->m_Size;
				*nextPointer.asSize_tPtr = reinterpret_cast<size_t>(pPrevHeader);
			}
		}
	}


	HeapAllocator::HeapHeader *HeapAllocator::CreateUsedHeader(const size_t size)
	{
		HeapHeader *pFreeHeader = m_pFreeHeapList;
		while (pFreeHeader)
		{
			if (pFreeHeader->m_Size >= size)
			{
				break;
			}

			pFreeHeader = pFreeHeader->m_pNext;
		}

		HeapHeader *pNewHeader = nullptr;
		if (pFreeHeader)
		{
			RemoveHeader(pFreeHeader);
			
			pNewHeader = pFreeHeader;
			pNewHeader->m_Used = true;

			size_t remainingSpace = pFreeHeader->m_Size - size;
			if (remainingSpace >= MIN_HEAP_BLOCK_SIZE)
			{
				MemoryAddress nextAddress(pFreeHeader);
				nextAddress.asSize_t += sizeof(HeapHeader) + size;
				*nextAddress.asSize_tPtr = reinterpret_cast<size_t>(pFreeHeader);
				nextAddress.asSize_tPtr++;

				CreateFreeHeader(nextAddress.asVoidPtr, remainingSpace - sizeof(HeapHeader) - sizeof(size_t));

				pNewHeader->m_Size = size;
			}

			pNewHeader->m_pNext = m_pUsedHeapList;
			if (m_pUsedHeapList)
			{
				m_pUsedHeapList->m_pPrev = pNewHeader;
			}
			m_pUsedHeapList = pNewHeader;
		}

		return pNewHeader;
	}


	void HeapAllocator::RemoveHeader(HeapHeader * const pHeader)
	{
		if (pHeader->m_pNext)
		{
			pHeader->m_pNext->m_pPrev = pHeader->m_pPrev;
		}

		if (pHeader->m_pPrev)
		{
			pHeader->m_pPrev->m_pNext = pHeader->m_pNext;
		}

		if (pHeader == m_pFreeHeapList)
		{
			m_pFreeHeapList = m_pFreeHeapList->m_pNext;
		}

		if (pHeader == m_pUsedHeapList)
		{
			m_pUsedHeapList = m_pUsedHeapList->m_pNext;
		}

		pHeader->m_pPrev = nullptr;
		pHeader->m_pNext = nullptr;
	}
}