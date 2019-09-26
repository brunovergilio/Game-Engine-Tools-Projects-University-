#include "AsyncIO.h"
#include "MemoryHelpers.h"
#include "File.h"

namespace BVFileIO
{
	AsyncIO::AsyncIO()
		: m_hFileIOThread(nullptr), m_hFileIOSemaphore(nullptr), m_pAllocator(nullptr), m_OpID(0U), m_Running(false)
	{
	}


	AsyncIO::~AsyncIO()
	{
	}


	FileResult AsyncIO::StartUp(const unsigned int maxQueueSize, BVMemory::Allocator * const pAllocator)
	{
		FileResult result = FileResult::SUCCESS;

		m_pAllocator = pAllocator;

		if (FileResult::SUCCESS == result)
		{
			HANDLE hSem;
			AsyncIOData *pData = nullptr;
			for (unsigned int i = 0; i < maxQueueSize; i++)
			{
				hSem = CreateSemaphore(nullptr, 0, 1, nullptr);
				if (m_pAllocator)
				{
					pData = BV_NEW_A(AsyncIOData, 16, m_pAllocator) AsyncIOData();
				}
				else
				{
					pData = BV_NEW(AsyncIOData, 16) AsyncIOData();
				}
				pData->m_hSemaphore = hSem;

				m_FreeQueue.Enqueue(pData);
			}
		}

		if (FileResult::SUCCESS == result)
		{
			m_hFileIOThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, AsyncIO::IOThread, this, CREATE_SUSPENDED, nullptr));
			if (!m_hFileIOThread)
			{
				result = FileResult::ASYNC_FAIL;
			}
		}

		if (FileResult::SUCCESS == result)
		{
			m_hFileIOSemaphore = CreateSemaphore(nullptr, 0, maxQueueSize, nullptr);
			if (!m_hFileIOSemaphore)
			{
				CloseHandle(m_hFileIOThread);
				result = FileResult::ASYNC_FAIL;
			}
		}

		if (FileResult::SUCCESS == result)
		{
			m_Running = true;

			InitializeCriticalSection(&m_CriticalSection);

			ResumeThread(m_hFileIOThread);
		}

		return result;
	}


	FileResult AsyncIO::ShutDown()
	{
		FileResult result = FileResult::SUCCESS;

		if (m_Running)
		{
			m_Running = false;

			ReleaseSemaphore(m_hFileIOSemaphore, 1, nullptr);

			WaitForSingleObject(m_hFileIOThread, INFINITE);

			CloseHandle(m_hFileIOThread);

			DeleteCriticalSection(&m_CriticalSection);

			CloseHandle(m_hFileIOSemaphore);

			while (!m_FreeQueue.IsEmpty())
			{
				AsyncIOData *pData = m_FreeQueue.Dequeue();
				if (pData->m_hSemaphore != INVALID_HANDLE_VALUE)
				{
					CloseHandle(pData->m_hSemaphore);
				}

				if (m_pAllocator)
				{
					BV_DELETE_A(pData, m_pAllocator);
				}
				else
				{
					BV_DELETE(pData);
				}
			}
		}
		else
		{
			result = FileResult::ASYNC_FAIL;
		}

		return result;
	}


	void AsyncIO::ProcessAsyncIO()
	{
		while (m_Running)
		{
			WaitForSingleObject(m_hFileIOSemaphore, INFINITE);

			if (!m_UsedQueue.IsEmpty())
			{
				EnterCriticalSection(&m_CriticalSection);

				AsyncIOData *pData = m_UsedQueue.Dequeue();

				size_t bytes = 0U;
				if (pData->m_OpType == AsyncOpType::READ)
				{
					pData->m_File->Read(pData->m_pBuffer, pData->m_BufferLength, &bytes);
				}
				else
				{
					pData->m_File->Write(pData->m_pBuffer, pData->m_BufferLength, &bytes);
				}

				if (pData->m_pCallback != nullptr)
				{
					pData->m_pCallback(bytes, pData->m_pArgs);
				}

				pData->m_Done = true;
				ReleaseSemaphore(pData->m_hSemaphore, 1, nullptr);

				m_FreeQueue.Enqueue(pData);

				LeaveCriticalSection(&m_CriticalSection);
			}
		}
	}


	THREAD AsyncIO::IOThread(void *pArgs)
	{
		(reinterpret_cast<AsyncIO *>(pArgs))->ProcessAsyncIO();
		
		return 0;
	}


	AsyncIOResult AsyncIO::ReadAsync(File & file, void * const pBuffer, const size_t bufferLength, AsyncIOCallback readCallback, void *pArgs)
	{
		AsyncIOData *pData = nullptr;

		EnterCriticalSection(&m_CriticalSection);
		if (!m_FreeQueue.IsEmpty() && m_Running)
		{
			pData = m_FreeQueue.Dequeue();
			
			pData->m_File = &file;
			pData->m_pBuffer = pBuffer;
			pData->m_BufferLength = bufferLength;
			pData->m_pCallback = readCallback;
			pData->m_pArgs = pArgs;
			pData->m_OpType = AsyncOpType::READ;
			pData->m_Done = false;

			pData->m_ID = m_OpID++;

			m_UsedQueue.Enqueue(pData);

			ReleaseSemaphore(m_hFileIOSemaphore, 1, nullptr);
		}
		LeaveCriticalSection(&m_CriticalSection);

		return AsyncIOResult(pData);
	}


	AsyncIOResult AsyncIO::WriteAsync(File & file, void * const pBuffer, const size_t bufferLength, AsyncIOCallback writeCallback, void *pArgs)
	{
		AsyncIOData *pData = nullptr;

		EnterCriticalSection(&m_CriticalSection);
		if (!m_FreeQueue.IsEmpty() && m_Running)
		{

			pData = m_FreeQueue.Dequeue();

			pData->m_File = &file;
			pData->m_pBuffer = pBuffer;
			pData->m_BufferLength = bufferLength;
			pData->m_pCallback = writeCallback;
			pData->m_pArgs = pArgs;
			pData->m_OpType = AsyncOpType::WRITE;
			pData->m_Done = false;

			pData->m_ID = m_OpID++;

			m_UsedQueue.Enqueue(pData);

			ReleaseSemaphore(m_hFileIOSemaphore, 1, nullptr);

		}
		LeaveCriticalSection(&m_CriticalSection);

		return AsyncIOResult(pData);
	}

	FileResult AsyncIO::WaitAll()
	{
		FileResult result = FileResult::SUCCESS;

		if (m_Running)
		{
			AsyncIOData *pData = m_UsedQueue.GetFirst();
			while (pData)
			{
				AsyncIOResult r(pData);
				r.Wait();
				pData = m_UsedQueue.GetFirst();
			}
		}
		else
		{
			result = FileResult::ASYNC_IO_FAIL;
		}

		return result;
	}
}