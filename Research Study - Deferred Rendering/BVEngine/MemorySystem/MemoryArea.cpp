#include "MemoryArea.h"

#include "MemoryUtils.h"

namespace BVMemory
{
	MemoryArea::MemoryArea()
		: m_pStart(nullptr), m_pEnd(nullptr)
	{
	}


	MemoryArea::~MemoryArea()
	{
		Destroy();
	}


	void MemoryArea::Create(const size_t size)
	{
		BV_ASSERT(size > 0, "MemoryArea::Create -> The size parameter must be bigger than 0");

		Destroy();

		// Get the page size
		// VirtualAlloc will round the number to a multiple of the page size, so we need the correct values
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		const size_t pageSize = systemInfo.dwPageSize;

		const size_t totalSize = (size % pageSize == 0 ? size / pageSize : size / pageSize + 1) * pageSize;
		char * pMem = (char *)VirtualAlloc(nullptr, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		//BV_ASSERT(m_pStart != nullptr, "MemoryArea::Create -> Allocation failed");
		if (pMem)
		{
			m_pStart = pMem;
			m_pEnd = m_pStart + totalSize;
		}
	}


	void MemoryArea::Destroy()
	{
		if (m_pStart)
		{
			VirtualFree(m_pStart, 0, MEM_RELEASE);
			m_pStart = nullptr;
			m_pEnd = nullptr;
		}
	}
}