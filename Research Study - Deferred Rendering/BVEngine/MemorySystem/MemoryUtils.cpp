#include "MemoryUtils.h"

namespace BVMemory
{
	void *Align(void * const pMem, const size_t alignment, const size_t bytesToAlignAfter)
	{
		BV_ASSERT(pMem != nullptr, "MemoryUtils::Align -> Parameter pMem cannot be null");
		BV_ASSERT((alignment & (alignment - 1)) == 0, "MemoryUtils::Align -> Alignment must be a power of 2");
		BV_ASSERT(alignment >= 1, "MemoryUtils::Align -> Alignment must be greater than or equal to 1");
		BV_ASSERT(alignment <= 128, "MemoryUtils::Align -> Alignment must be less than or equal to 128");

		MemoryAddress memAddress(pMem);
		memAddress.asSize_t += bytesToAlignAfter;

		size_t mask = alignment - 1;
		size_t misalignment = mask & memAddress.asSize_t;
		size_t adjustment = alignment - misalignment;

		memAddress.asSize_t += adjustment - bytesToAlignAfter;
		memAddress.asCharPtr[-1] = static_cast<char>(adjustment);

		return memAddress.asVoidPtr;
	}


	void *AllocateAligned(const size_t size, const size_t alignment, const size_t bytesToAlignAfter)
	{
		return Align(malloc(size + alignment + bytesToAlignAfter), alignment, bytesToAlignAfter);
	}


	void FreeAligned(void * const pAddress)
	{
		char *p = reinterpret_cast<char *>(pAddress);
		p -= p[-1];
		free(p);
	}

	void * AllocateUnaligned(const size_t size)
	{
		return malloc(size);
	}

	void FreeUnaligned(void * const pAddress)
	{
		free(pAddress);
	}

	unsigned int GetNextPowerOf2(const unsigned int size)
	{
		unsigned int newSize = 1;
		while (newSize < size)
		{
			newSize <<= 1;
		}

		return newSize;
	}
}