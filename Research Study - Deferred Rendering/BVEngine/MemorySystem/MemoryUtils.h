#ifndef MEMORYUTILS
#define MEMORYUTILS

// Standard C / C++ Libraries
#include <new>
#include <cassert>

// Windows Libraries
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Type definitions

// Macros

// Hack for assert to show a message
#define BV_ASSERT(condition, message) assert((message, condition))
//#define BV_ASSERT(condition, message) assert(condition && message) // Another way to do it

#define BV_UNUSED(var) var

namespace BVMemory
{
	union MemoryAddress
	{
		size_t asSize_t;
		size_t *asSize_tPtr;
		void *asVoidPtr;
		char *asCharPtr;
		int *asIntPtr;
		unsigned int *asUIntPtr;

		MemoryAddress() : asSize_t(0) {}
		MemoryAddress(const size_t address) : asSize_t(address) {}
		MemoryAddress(size_t * const pAddress) : asSize_tPtr(pAddress) {}
		MemoryAddress(void * const pAddress) : asVoidPtr(pAddress) {}
		MemoryAddress(char * const pAddress) : asCharPtr(pAddress) {}
		MemoryAddress(int * const pAddress) : asIntPtr(pAddress) {}
	};
	
	void *Align(void * const pMem, const size_t alignment, const size_t bytesToAlignAfter = 0);

	void *AllocateAligned(const size_t size, const size_t alignment, const size_t bytesToAlignAfter = 0);
	void FreeAligned(void * const pAddress);

	void *AllocateUnaligned(const size_t size);
	void FreeUnaligned(void * const pAddress);

	unsigned int GetNextPowerOf2(const unsigned int size);
}
#endif