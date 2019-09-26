#include "AsyncIOResult.h"

namespace BVFileIO
{
	AsyncIOResult::AsyncIOResult(AsyncIOData * const pAsyncIOData)
		: m_pAsyncIOData(pAsyncIOData)
	{
		if (pAsyncIOData)
		{
			m_ID = pAsyncIOData->m_ID;
		}
	}


	AsyncIOResult::AsyncIOResult(const AsyncIOResult & other)
		: m_pAsyncIOData(other.m_pAsyncIOData), m_ID(other.m_ID)
	{
	}


	AsyncIOResult & AsyncIOResult::operator = (const AsyncIOResult & other)
	{
		if (this != &other)
		{
			m_pAsyncIOData = other.m_pAsyncIOData;
			m_ID = other.m_ID;
		}

		return *this;
	}


	AsyncIOResult::~AsyncIOResult()
	{
		m_pAsyncIOData = nullptr;
		m_ID = 0U;
	}


	bool AsyncIOResult::IsDone()
	{
		bool isDone = false;
		if (m_pAsyncIOData)
		{
			isDone = m_ID != m_pAsyncIOData->m_ID ? true : m_pAsyncIOData->m_Done;
		}

		return isDone;
	}


	void AsyncIOResult::Wait()
	{
		if (!IsDone() && m_pAsyncIOData)
		{
			WaitForSingleObject(m_pAsyncIOData->m_hSemaphore, INFINITE);
		}
	}

	FileResult AsyncIOResult::GetStatus()
	{
		return (m_pAsyncIOData) ? FileResult::SUCCESS : FileResult::ASYNC_IO_FAIL;
	}
}