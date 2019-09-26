#ifndef ASYNCIORESULT_H
#define ASYNCIORESULT_H

#include "FileUtils.h"

namespace BVFileIO
{
	class File;

	enum class AsyncOpType : int
	{
		READ,
		WRITE
	};

	typedef void(*AsyncIOCallback)(const size_t bytes, void *pArgs);

	struct AsyncIOData
	{
		AsyncIOData *m_pNext;
		AsyncIOData *m_pPrev;

		HANDLE m_hSemaphore;
		File *m_File;
		void *m_pBuffer;
		size_t m_BufferLength;
		AsyncIOCallback m_pCallback;
		void *m_pArgs;
		AsyncOpType m_OpType;
		unsigned int m_ID;
		bool m_Done;

		AsyncIOData()
			: m_hSemaphore(INVALID_HANDLE_VALUE), m_File(nullptr), m_pBuffer(nullptr),
			m_BufferLength(0U), m_OpType(AsyncOpType::READ), m_pCallback(nullptr), m_pArgs(nullptr),
			m_ID(0U), m_Done(true), m_pNext(nullptr), m_pPrev(nullptr)
		{
		}
	};

	class AsyncIOResult
	{
	public:
		AsyncIOResult(AsyncIOData * const pAsyncIOData);
		AsyncIOResult(const AsyncIOResult & other);
		AsyncIOResult & operator = (const AsyncIOResult & other);
		~AsyncIOResult();

		bool IsDone();
		void Wait();
		FileResult GetStatus();

	private:
		AsyncIOResult() = delete;

	private:
		AsyncIOData *m_pAsyncIOData;
		unsigned int m_ID;
	};
}
#endif