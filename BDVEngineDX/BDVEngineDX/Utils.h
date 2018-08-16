#ifndef UTILS_H
#define UTILS_H

#define WIN32_LEAN_AND_MEAN

#define USE_DIRECTX

//Windows and DirectX libraries
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl\client.h>

//DirectX linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

//C++ Standard libraries
#include <cstdio>

//Namespaces
using namespace DirectX;
using namespace Microsoft::WRL;

//Typedefs
typedef signed char        i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef float			   f32;
typedef double			   d64;

#endif // !UTILS_H