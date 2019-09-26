#ifndef MEMORYHELPERS_H
#define MEMORYHELPERS_H

#include "MemoryUtils.h"
#include "Allocator.h"

namespace BVMemory
{
	template<class T>
	void *New(const bool aligned = false, const size_t alignment = 0, Allocator * const pAllocator = nullptr)
	{
		void *pMem = nullptr;
		if (pAllocator)
		{
			if (aligned)
			{
				pMem = pAllocator->AllocateA(sizeof(T), alignment);
			}
			else
			{
				pMem = pAllocator->AllocateU(sizeof(T));
			}
		}
		else
		{
			if (aligned)
			{
				pMem = AllocateAligned(sizeof(T), alignment);
			}
			else
			{
				pMem = AllocateUnaligned(sizeof(T));
			}
		}

		return pMem;
	};

	template<class T>
	void Delete(T * const pObject, const bool aligned = false, Allocator * const pAllocator = nullptr)
	{
		pObject->~T();
		if (pAllocator)
		{
			if (aligned)
			{
				pAllocator->FreeA(pObject);
			}
			else
			{
				pAllocator->FreeU(pObject);
			}
		}
		else
		{
			if (aligned)
			{
				FreeAligned(pObject);
			}
			else
			{
				FreeUnaligned(pObject);
			}
		}
	};

	template<class T>
	T *NewArray(const size_t count, const bool aligned = false, const size_t alignment = 0, Allocator * const pAllocator = nullptr)
	{
		const size_t totalSize = count * sizeof(T);

		MemoryAddress pMem;
		if (pAllocator)
		{
			if (aligned)
			{
				pMem.asVoidPtr = pAllocator->AllocateA(totalSize, alignment, sizeof(size_t));
			}
			else
			{
				pMem.asVoidPtr = pAllocator->AllocateU(totalSize + sizeof(size_t));
			}
		}
		else
		{
			if (aligned)
			{
				pMem.asVoidPtr = AllocateAligned(totalSize, alignment, sizeof(size_t));
			}
			else
			{
				pMem.asVoidPtr = AllocateUnaligned(totalSize + sizeof(size_t));
			}
		}

		pMem.asSize_tPtr[0] = count;
		pMem.asSize_tPtr++;

		{
			const size_t sizeOfT = sizeof(T);
			const size_t end = pMem.asSize_t + totalSize;
			while (pMem.asSize_t < end)
			{
				new(pMem.asVoidPtr) T();
				pMem.asSize_t += sizeOfT;
			}
			pMem.asSize_t -= totalSize;
		}

		return reinterpret_cast<T *>(pMem.asVoidPtr);
	};

	template<class T>
	void DeleteArray(T * const pObjects, const bool aligned = false, Allocator * const pAllocator = nullptr)
	{
		MemoryAddress pMem(pObjects);
		const size_t count = pMem.asSize_tPtr[-1];

		for (size_t i = count; i > 0; i--)
		{
			pObjects[i - 1].~T();
		}


		pMem.asSize_tPtr--;
		if (pAllocator)
		{
			if (aligned)
			{
				pAllocator->FreeA(pMem.asVoidPtr);
			}
			else
			{
				pAllocator->FreeU(pMem.asVoidPtr);
			}
		}
		else
		{
			if (aligned)
			{
				FreeAligned(pMem.asVoidPtr);
			}
			else
			{
				FreeUnaligned(pMem.asVoidPtr);
			}
		}
	};

// Quick macros for allocating memory

// Unaligned
#define BV_NEW(type, allocator) new(BVMemory::New<type>(false, 0, allocator))
#define BV_DELETE(object, allocator) BVMemory::Delete(object, false, allocator)
#define BV_NEW_ARRAY(type, count, allocator) BVMemory::NewArray<type>(count, false, 0, allocator)
#define BV_DELETE_ARRAY(objects, allocator) BVMemory::DeleteArray(objects, false, allocator)

// Aligned
#define BV_NEW_A(type, alignment, allocator) new(BVMemory::New<type>(true, alignment, allocator))
#define BV_DELETE_A(object, allocator) BVMemory::Delete(object, true, allocator)
#define BV_NEW_ARRAY_A(type, count, alignment, allocator) BVMemory::NewArray<type>(count, true, alignment, allocator)
#define BV_DELETE_ARRAY_A(objects, allocator) BVMemory::DeleteArray(objects, true, allocator)
}

#endif