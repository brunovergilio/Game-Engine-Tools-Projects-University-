#include "SafeHeapAllocator.h"


namespace BVMemory
{
	SafeHeapAllocator::SafeHeapAllocator()
		: HeapAllocator()
	{
		InitializeCriticalSection(&m_CS);
	}


	SafeHeapAllocator::SafeHeapAllocator(const size_t size)
		: HeapAllocator(size)
	{
		InitializeCriticalSection(&m_CS);
	}


	SafeHeapAllocator::SafeHeapAllocator(void * const pStart, void * const pEnd)
		: HeapAllocator(pStart, pEnd)
	{
		InitializeCriticalSection(&m_CS);
	}


	SafeHeapAllocator::~SafeHeapAllocator()
	{
		DeleteCriticalSection(&m_CS);
	}


	void * SafeHeapAllocator::AllocateA(const size_t size, const size_t alignment, const size_t bytesToAlignAfter)
	{
		EnterCriticalSection(&m_CS);
		void * pMem = HeapAllocator::AllocateA(size, alignment, bytesToAlignAfter);
		LeaveCriticalSection(&m_CS);

		return pMem;
	}


	void SafeHeapAllocator::FreeA(void * const pAddress)
	{
		EnterCriticalSection(&m_CS);
		HeapAllocator::FreeA(pAddress);
		LeaveCriticalSection(&m_CS);
	}


	void * SafeHeapAllocator::AllocateU(const size_t size)
	{
		EnterCriticalSection(&m_CS);
		void * pMem = HeapAllocator::AllocateU(size);
		LeaveCriticalSection(&m_CS);

		return pMem;
	}


	void SafeHeapAllocator::FreeU(void * const pAddress)
	{
		EnterCriticalSection(&m_CS);
		HeapAllocator::FreeU(pAddress);
		LeaveCriticalSection(&m_CS);
	}


	void SafeHeapAllocator::Clear()
	{
		EnterCriticalSection(&m_CS);
		HeapAllocator::Clear();
		LeaveCriticalSection(&m_CS);
	}
}