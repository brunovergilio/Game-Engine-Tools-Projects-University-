#ifndef ASYNCIOQUEUE_H
#define ASYNCIOQUEUE_H

#include "AsyncIOResult.h"

namespace BVFileIO
{
	class AsyncIOQueue
	{
	public:
		AsyncIOQueue();
		~AsyncIOQueue();

		void Enqueue(AsyncIOData * const pData);
		AsyncIOData *Dequeue();
		bool IsEmpty();
		AsyncIOData *GetFirst();

	private:
		AsyncIOData *m_pHead;
	};
}
#endif