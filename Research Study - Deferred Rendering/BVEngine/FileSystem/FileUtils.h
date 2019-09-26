#ifndef FILEUTILS_H
#define FILEUTILS_H

// Standard C / C++ Libraries
#include <cassert>
#include <utility>

// Windows Libraries
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>

// Type definitions

// Macros
#define BVASSERT(condition, message) assert((message, condition))
#define THREAD unsigned CALLBACK


// Enums
enum class FileResult : int
{
	SUCCESS,
	OPEN_FAIL,
	CLOSE_FAIL,
	READ_FAIL,
	READ_EOF,
	WRITE_FAIL,
	FLUSH_FAIL,
	SEEK_FAIL,
	SIZE_FAIL,
	ASYNC_FAIL,
	ASYNC_IO_FAIL,
};

#endif