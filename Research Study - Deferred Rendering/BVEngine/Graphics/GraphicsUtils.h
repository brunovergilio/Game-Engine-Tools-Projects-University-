#ifndef GRAPHICSUTILS
#define GRAPHICSUTILS

//Windows and DirectX libraries
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11_2.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl\client.h>

// Standard C / C++ Libraries
#include <cstdio>
#include <memory>
#include <vector>
#include <cassert>

// Namespaces
using namespace Microsoft::WRL;

// Linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

// Macros

// Hack for assert to show a message
#define BV_ASSERT(condition, message) assert((message, condition))
//#define BV_ASSERT(condition, message) assert(condition && message) // Another way to do it

#define BV_UNUSED(var) var
#define BV_SAFE_DELETE(p) { if (p) { delete p; p = nullptr; } }
#define BV_SAFE_DELETE_ARRAY(p) { if (p) { delete[] p; p = nullptr; } }
#define BV_RELEASE(pCOM) { if (pCOM) { pCOM->Release(); pCOM = nullptr; } }

template<class T>
inline T** ReleaseAndReturnAddress(T * & pCom)
{
	BV_RELEASE(pCOM);
	return &pCom;
}

#define BV_RELEASE_AND_RETURN(pCOM) ReleaseAndReturnAddress(pCOM)

// Enums
enum class GraphicsResult : int
{
	SUCCESS,
	WINDOW_FAIL,
	GRAPHICS_FAIL,
	HARDWARE_BUFFER_FAIL,
	SHADER_FAIL,
	RASTERIZER_STATE_FAIL,
	BLEND_STATE_FAIL,
	DEPTH_STENCIL_STATE_FAIL,
	SAMPLER_STATE_FAIL,
	SHADER_RESOURCE_VIEW_FAIL,
	RENDER_TARGET_VIEW_FAIL,
	DEPTH_STENCIL_VIEW_FAIL,
	TEXTURE_FAIL
};

#endif