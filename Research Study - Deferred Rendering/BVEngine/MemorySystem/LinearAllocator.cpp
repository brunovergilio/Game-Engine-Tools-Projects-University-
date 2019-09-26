#include "LinearAllocator.h"

#include "MemoryUtils.h"

namespace BVMemory
{
	LinearAllocator::LinearAllocator()
		: m_pStart(nullptr), m_pCurrent(nullptr), m_pEnd(nullptr)
	{
	}


	LinearAllocator::LinearAllocator(const size_t size)
		: m_pStart(nullptr), m_pCurrent(nullptr), m_pEnd(nullptr)
	{
		Create(size);
	}


	LinearAllocator::LinearAllocator(void * const pStart, void * const pEnd)
		: m_pStart(reinterpret_cast<char *>(pStart)), m_pCurrent(m_pStart), m_pEnd(reinterpret_cast<char *>(pEnd))
	{
		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "LinearAllocator::LinearAllocator -> Memory parameters must not be null");
	}


	LinearAllocator::~LinearAllocator()
	{
		m_pStart = nullptr;
		m_pCurrent = nullptr;
		m_pEnd = nullptr;
	}


	void LinearAllocator::Create(const size_t size)
	{
		BV_ASSERT(size > 0, "LinearAllocator::Create -> Size cannot be 0");

		m_MemoryArea.Create(size);

		m_pStart = m_MemoryArea.GetStart();
		m_pCurrent = m_pStart;
		m_pEnd = m_MemoryArea.GetEnd();

		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "LinearAllocator::Create -> Memory parameters must not be null");
	}


	void LinearAllocator::Assign(void * const pStart, void * const pEnd)
	{
		BV_ASSERT(pStart != nullptr && pEnd != nullptr, "LinearAllocator::Assign -> Memory parameters must not be null");

		m_pStart = reinterpret_cast<char *>(pStart);
		m_pCurrent = m_pStart;
		m_pEnd = reinterpret_cast<char *>(pEnd);
	}


	void *LinearAllocator::AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter)
	{
		// Align the memory address to the specified alignment
		void *pMem = Align(m_pCurrent, alignment, bytesToAlignAfter);

		// Check to see if there's still enough space after the alignment
		// If the nextBlock is bigger than m_pEnd then we're going past bounds
		char *pNextBlock = reinterpret_cast<char *>(pMem) + size;
		if (pNextBlock > m_pEnd)
		{
			return nullptr;
		}

		// Update the current pointer
		m_pCurrent = pNextBlock;

		return pMem;
	}


	void *LinearAllocator::AllocateU(const size_t size)
	{
		void *pMem = m_pCurrent;

		// Check to see if there's still enough space after the alignment
		// If the nextBlock is bigger than m_pEnd then we're going past bounds
		char *pNextBlock = reinterpret_cast<char *>(pMem) + size;
		if (pNextBlock > m_pEnd)
		{
			return nullptr;
		}

		// Update the current pointer
		m_pCurrent = pNextBlock;

		return pMem;
	}


	void LinearAllocator::Clear()
	{
		m_pCurrent = m_pStart;
	}
}