#include "StackAllocator.h"

#include "MemoryUtils.h"

namespace BVMemory
{
	StackAllocator::StackAllocator()
		: m_pStart(nullptr), m_pCurrent(nullptr), m_pEnd(nullptr), m_LastAllocation(0)
	{
	}


	StackAllocator::StackAllocator(const size_t size)
		: m_pStart(nullptr), m_pCurrent(nullptr), m_pEnd(nullptr), m_LastAllocation(0)
	{
		Create(size);
	}


	StackAllocator::StackAllocator(void *pStart, void *pEnd)
		: m_pStart(reinterpret_cast<char *>(pStart)), m_pCurrent(m_pStart), m_pEnd(reinterpret_cast<char *>(pEnd)), m_LastAllocation(0)
	{
		BV_ASSERT(m_pStart != nullptr && m_pEnd != nullptr, "StackAllocator::StackAllocator -> Memory parameters must not be null");
	}


	StackAllocator::~StackAllocator()
	{
		m_pStart = nullptr;
		m_pCurrent = nullptr;
		m_pEnd = nullptr;
		m_LastAllocation = 0;
	}


	void StackAllocator::Create(const size_t size)
	{
		BV_ASSERT(size > 0, "StackAllocator::Create -> Size cannot be 0");

		m_MemoryArea.Create(size);

		m_pStart = reinterpret_cast<char *>(m_MemoryArea.GetStart());
		m_pCurrent = m_pStart;
		m_pEnd = reinterpret_cast<char *>(m_MemoryArea.GetEnd());
	}


	void StackAllocator::Assign(void * const pStart, void * const pEnd)
	{
		BV_ASSERT(pStart != nullptr && pEnd != nullptr, "StackAllocator::Assign -> Memory parameters must not be null");
	}


	void *StackAllocator::AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter)
	{
		// Store the current position in a temp value to add the allocation number later
		unsigned int *pCurrentAllocation = reinterpret_cast<unsigned int *>(m_pCurrent);

		// Align the memory address to the specified alignment
		// Add an extra sizeof(unsigned int) bytes, to store the id of the last allocation
		void *pMem = Align(m_pCurrent + sizeof(unsigned int), alignment, bytesToAlignAfter);

		// Check to see if there's still enough space after the alignment
		// If the nextBlock is bigger than m_pEnd then we're going past bounds
		char *pNextBlock = reinterpret_cast<char *>(pMem) + size;
		if (pNextBlock > m_pEnd)
		{
			return nullptr;
		}

		// Update the current pointer
		m_pCurrent = pNextBlock;

		// Increment and store the last allocation made
		*pCurrentAllocation = ++m_LastAllocation;

		// Return the aligned memory
		return pMem;
	}


	void StackAllocator::FreeA(void * const pAddress)
	{
		BV_ASSERT(pAddress != nullptr, "StackAllocator::Free -> Address supplied cannot be null");

		// Get the address
		MemoryAddress currAddress(pAddress);
		// Move it back by the number of aligned bytes (store in one byte before the address)
		currAddress = currAddress.asCharPtr - currAddress.asCharPtr[-1];
		// And get the allocation number from sizeof(int) bytes back
		unsigned int numAllocation = currAddress.asUIntPtr[-1];

		// Make sure this allocation is the last one
		BV_ASSERT(numAllocation == m_LastAllocation, "StackAllocator::Free -> Address supplied is not the last allocation made");

		// Nothing changes if this is not the last allocation
		if (numAllocation != m_LastAllocation)
		{
			return;
		}

		// Move the address back by sizeof(int) bytes and assign it to the m_pCurrent
		currAddress.asUIntPtr--;
		m_pCurrent = currAddress.asCharPtr;

		// Decrement the allocation number
		m_LastAllocation--;
	}


	void *StackAllocator::AllocateU(const size_t size)
	{
		// Store the current position in a temp value to add the allocation number later
		unsigned int *pCurrentAllocation = reinterpret_cast<unsigned int *>(m_pCurrent);

		// Add an extra sizeof(unsigned int) bytes, to store the id of the last allocation
		void *pMem = m_pCurrent + sizeof(unsigned int);

		// Check to see if there's still enough space after the alignment
		// If the nextBlock is bigger than m_pEnd then we're going past bounds
		char *pNextBlock = reinterpret_cast<char *>(pMem) + size;
		if (pNextBlock > m_pEnd)
		{
			return nullptr;
		}

		// Update the current pointer
		m_pCurrent = pNextBlock;

		// Increment and store the last allocation made
		*pCurrentAllocation = ++m_LastAllocation;

		// Return the aligned memory
		return pMem;
	}


	void StackAllocator::FreeU(void * const pAddress)
	{
		BV_ASSERT(pAddress != nullptr, "StackAllocator::FreeU -> Address supplied cannot be null");

		// Get the address
		MemoryAddress currAddress(pAddress);
		// And get the allocation number from sizeof(int) bytes back
		unsigned int numAllocation = currAddress.asUIntPtr[-1];

		// Make sure this allocation is the last one
		BV_ASSERT(numAllocation == m_LastAllocation, "StackAllocator::FreeU -> Address supplied is not the last allocation made");

		// Nothing changes if this is not the last allocation
		if (numAllocation != m_LastAllocation)
		{
			return;
		}

		// Move the address back by sizeof(unsigned int) bytes and assign it to the m_pCurrent
		currAddress.asUIntPtr--;
		m_pCurrent = currAddress.asCharPtr;

		// Decrement the allocation number
		m_LastAllocation--;
	}


	void StackAllocator::Clear()
	{
		m_pCurrent = m_pStart;
		m_LastAllocation = 0;
	}
}