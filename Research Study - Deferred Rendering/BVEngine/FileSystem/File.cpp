#include "File.h"

namespace BVFileIO
{
	AsyncIO File::m_AsyncIO;

	File::File()
		: m_hFile(INVALID_HANDLE_VALUE)
	{
	}


	File::File(const File & other)
		: m_hFile(other.m_hFile)
	{
	}


	File::File(File && other)
	{
		*this = std::move(other);
	}


	File & File::operator =(const File & other)
	{
		if (this != &other)
		{
			m_hFile = other.m_hFile;
		}

		return *this;
	}


	File & File::operator =(File && other)
	{
		if (this != &other)
		{
			m_hFile = other.m_hFile;
			other.m_hFile = INVALID_HANDLE_VALUE;
		}

		return *this;
	}


	File::~File()
	{
		Close();
	}


	File::File(const char *pFileName, const FileMode mode)
		: m_hFile(INVALID_HANDLE_VALUE)
	{
		Open(pFileName, mode);
		//BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::File -> Error opening file");
	}


	// Basic operations
	FileResult File::Open(const char *pFileName, const FileMode mode)
	{
		BVASSERT(pFileName != nullptr, "File::Open -> Null filename");

		FileResult result = FileResult::SUCCESS;

		if (INVALID_HANDLE_VALUE != m_hFile)
		{
			Close();
		}

		DWORD acccessMode = 0;
		DWORD openMode = OPEN_ALWAYS;

		switch (mode)
		{
		case FileMode::READ:
			acccessMode = GENERIC_READ;
			openMode = OPEN_EXISTING;
			break;
		case FileMode::WRITE:
			acccessMode = GENERIC_WRITE;
			break;
		case FileMode::READ_AND_WRITE:
			acccessMode = GENERIC_READ | GENERIC_WRITE;
			break;
		default:
			break;
		}

		m_hFile = CreateFileA(pFileName, acccessMode, 0, nullptr, openMode, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER fileSize;
			GetFileSizeEx(m_hFile, &fileSize);

			BVASSERT(fileSize.HighPart == 0, "File::Open -> File is bigger than the 32 bit limit");

			if (fileSize.HighPart > 0)
			{
				result = FileResult::OPEN_FAIL;
			}
		}
		else
		{
			result = FileResult::OPEN_FAIL;
		}

		return result;
	}


	FileResult File::Close()
	{
		FileResult result = FileResult::SUCCESS;

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		else
		{
			result = FileResult::CLOSE_FAIL;
		}

		return result;
	}


	FileResult File::Read(void * const pBuffer, const size_t bufferLength, size_t * const pBytesRead)
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::Read -> Invalid handle");
		BVASSERT(pBuffer != nullptr, "File::Read -> Null buffer was provided");
		BVASSERT(bufferLength > 0, "File::Read -> Buffer length must be bigger than 0");

		FileResult result = FileResult::SUCCESS;

		size_t pBytes = 0;
		if (ReadFile(m_hFile, pBuffer, bufferLength, (LPDWORD)&pBytes, nullptr))
		{
			if (pBytesRead)
			{
				*pBytesRead = pBytes;
				if (0 == pBytes)
				{
					result = FileResult::READ_EOF;
				}
			}
		}
		else
		{
			result = FileResult::READ_FAIL;
		}

		return result;
	}


	FileResult File::Write(const void * const pBuffer, const size_t bufferLength, size_t * const pBytesWritten)
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::Write -> Invalid handle");
		BVASSERT(pBuffer != nullptr, "File::Write -> Null buffer was provided");
		BVASSERT(bufferLength > 0, "File::Write -> Buffer length must be bigger than 0");

		FileResult result = FileResult::SUCCESS;

		size_t pBytes = 0;
		if (WriteFile(m_hFile, pBuffer, bufferLength, (LPDWORD)&pBytes, nullptr))
		{
			if (pBytesWritten)
			{
				*pBytesWritten = pBytes;
			}
		}
		else
		{
			result = FileResult::WRITE_FAIL;
		}

		return result;
	}


	FileResult File::Flush()
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::Flush -> Invalid handle");

		FileResult result = FileResult::SUCCESS;

		if (!FlushFileBuffers(m_hFile))
		{
			result = FileResult::FLUSH_FAIL;
		}

		return result;
	}


	FileResult File::SkipBytes(const int offset)
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::SkipBytes -> Invalid handle");

		FileResult result = FileResult::SUCCESS;

		if (SetFilePointer(m_hFile, offset, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
		{
			result = FileResult::SEEK_FAIL;
		}

		return result;
	}

	FileResult File::GoToStart()
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::GoToStart -> Invalid handle");

		FileResult result = FileResult::SUCCESS;

		if (SetFilePointer(m_hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			result = FileResult::SEEK_FAIL;
		}

		return result;
	}


	FileResult File::GoToEnd()
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::GoToEnd -> Invalid handle");

		FileResult result = FileResult::SUCCESS;

		if (SetFilePointer(m_hFile, 0, nullptr, FILE_END) == INVALID_SET_FILE_POINTER)
		{
			result = FileResult::SEEK_FAIL;
		}

		return result;
	}


	FileResult File::Seek(int offset, const FileSeek seekFrom)
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "Invalid handle");

		FileResult result = FileResult::SUCCESS;

		if (SetFilePointer(m_hFile, offset, nullptr, static_cast<DWORD>(seekFrom)) == INVALID_SET_FILE_POINTER)
		{
			result = FileResult::SEEK_FAIL;
		}

		return result;
	}


	size_t File::GetFilePos()
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::GetFilePos -> Invalid handle");

		size_t pos = SetFilePointer(m_hFile, 0, nullptr, FILE_CURRENT);
		return pos != INVALID_SET_FILE_POINTER ? pos : 0;
	}

	size_t File::GetSize()
	{
		BVASSERT(m_hFile != INVALID_HANDLE_VALUE, "File::GetSize() -> Invalid handle");

		LARGE_INTEGER fileSize;
		size_t size = 0;
		if (GetFileSizeEx(m_hFile, &fileSize))
		{
			size = fileSize.LowPart;
		}

		return size;
	}


	AsyncIOResult File::ReadAsync(void * const pBuffer, const size_t bufferLength, AsyncIOCallback readCallback, void *pArgs)
	{
		return m_AsyncIO.ReadAsync(*this, pBuffer, bufferLength, readCallback, pArgs);
	}

	AsyncIOResult File::WriteAsync(void * const pBuffer, const size_t bufferLength, AsyncIOCallback writeCallback, void *pArgs)
	{
		return m_AsyncIO.WriteAsync(*this, pBuffer, bufferLength, writeCallback, pArgs);
	}


	FileResult File::StartUpAsyncIO(const unsigned int maxQueueSize, BVMemory::Allocator * const pAllocator)
	{
		return m_AsyncIO.StartUp(maxQueueSize, pAllocator);
	}


	FileResult File::ShutDownAsyncIO()
	{
		return m_AsyncIO.ShutDown();
	}

	FileResult File::WaitAll()
	{
		return m_AsyncIO.WaitAll();
	}
}