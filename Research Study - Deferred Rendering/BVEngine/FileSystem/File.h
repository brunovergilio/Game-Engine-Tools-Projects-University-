#ifndef FILE_H
#define FILE_H

#include "AsyncIO.h"

namespace BVFileIO
{
	enum class FileMode : int
	{
		READ,
		WRITE,
		READ_AND_WRITE
	};

	enum class FileSeek : int
	{
		BEGIN,
		CURRENT,
		END
	};

	class File
	{
	public:
		File();
		File(const File & other);
		File(File && other);
		File & operator =(const File & other);
		File & operator =(File && other);
		virtual ~File();

		File(const char *pFileName, const FileMode mode = FileMode::READ_AND_WRITE);

		// Basic operations
		FileResult Open(const char *pFileName, const FileMode mode = FileMode::READ_AND_WRITE);
		FileResult Close();

		// Synchronous IO
		FileResult Read(void * const pBuffer, const size_t bufferLength, size_t * const pBytesRead = nullptr);
		FileResult Write(const void * const pBuffer, const size_t bufferLength, size_t * const pBytesWritten = nullptr);

		FileResult Flush();

		FileResult Seek(int offset, const FileSeek seekFrom);
		
		FileResult SkipBytes(const int offset);
		FileResult GoToStart();
		FileResult GoToEnd();
		
		size_t GetFilePos();

		size_t GetSize();

		AsyncIOResult ReadAsync(void * const pBuffer, const size_t bufferLength, AsyncIOCallback readCallback = nullptr, void *pArgs = nullptr);
		AsyncIOResult WriteAsync(void * const pBuffer, const size_t bufferLength, AsyncIOCallback writeCallback = nullptr, void *pArgs = nullptr);

		static FileResult StartUpAsyncIO(const unsigned int maxQueueSize = MAXIMUM_QUEUE_SIZE, BVMemory::Allocator * const pAllocator = nullptr);
		static FileResult ShutDownAsyncIO();

		static FileResult WaitAll();

	protected:
		HANDLE m_hFile;

		static AsyncIO m_AsyncIO;
	};
}
#endif