#include "AsyncIOQueue.h"

namespace BVFileIO
{
	AsyncIOQueue::AsyncIOQueue()
		: m_pHead(nullptr)
	{
	}


	AsyncIOQueue::~AsyncIOQueue()
	{
		m_pHead = nullptr;
	}

	void AsyncIOQueue::Enqueue(AsyncIOData * const pData)
	{
		if (m_pHead)
		{
			AsyncIOData *pLast = m_pHead->m_pPrev;
			pLast->m_pNext = pData;
			pData->m_pPrev = pLast;
			m_pHead->m_pPrev = pData;
		}
		else
		{
			m_pHead = pData;
			pData->m_pPrev = pData;
		}
	}


	AsyncIOData *AsyncIOQueue::Dequeue()
	{
		AsyncIOData *pData = nullptr;

		if (m_pHead)
		{
			pData = m_pHead;

			if (m_pHead->m_pNext)
			{
				m_pHead->m_pNext->m_pPrev = m_pHead->m_pPrev;
			}

			m_pHead = m_pHead->m_pNext;

			pData->m_pNext = nullptr;
			pData->m_pPrev = nullptr;
		}

		return pData;
	}

	bool AsyncIOQueue::IsEmpty()
	{
		return m_pHead == nullptr;
	}

	AsyncIOData *AsyncIOQueue::GetFirst()
	{
		return m_pHead;
	}
}