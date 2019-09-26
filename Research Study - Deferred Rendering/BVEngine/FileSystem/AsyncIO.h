#ifndef ASYNCIO_H
#define ASYNCIO_H

#include "AsyncIOQueue.h"
#include "Allocator.h"

namespace BVFileIO
{
	#define MAXIMUM_QUEUE_SIZE 10

	class AsyncIO
	{
	public:
		AsyncIO();
		~AsyncIO();

		FileResult StartUp(const unsigned int maxQueueSize = MAXIMUM_QUEUE_SIZE, BVMemory::Allocator * const pAllocator = nullptr);
		FileResult ShutDown();

		AsyncIOResult ReadAsync(File &file, void * const pBuffer, const size_t bufferLength, AsyncIOCallback readCallback = nullptr, void *pArgs = nullptr);
		AsyncIOResult WriteAsync(File &file, void * const pBuffer, const size_t bufferLength, AsyncIOCallback writeCallback = nullptr, void *pArgs = nullptr);

		FileResult WaitAll();

		void ProcessAsyncIO();

	private:
		static THREAD IOThread(void *pArgs);

		AsyncIO(const AsyncIO &other) = delete;
		AsyncIO & operator =(const AsyncIO &other) = delete;

	private:
		HANDLE m_hFileIOThread;
		HANDLE m_hFileIOSemaphore;
		BVMemory::Allocator * m_pAllocator;
		CRITICAL_SECTION m_CriticalSection;
		AsyncIOQueue m_UsedQueue;
		AsyncIOQueue m_FreeQueue;
		unsigned int m_OpID;
		bool m_Running;
	};
}
#endif