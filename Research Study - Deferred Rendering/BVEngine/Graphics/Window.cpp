#include "Window.h"


namespace BVGraphics
{
	Window::Window()
		: m_hWnd(nullptr)
	{
	}


	Window::~Window()
	{
	}


	GraphicsResult Window::Initialize(const char * const pWindowName, const int width, const int height, WNDPROC pWndproc, void * const pData)
	{
		GraphicsResult result = GraphicsResult::SUCCESS;

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = pWndproc;
		wndClass.cbClsExtra = NULL;
		wndClass.cbWndExtra = NULL;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = pWindowName;
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			result = GraphicsResult::WINDOW_FAIL;
		}


		if (GraphicsResult::SUCCESS == result)
		{
			RECT rect = { 0, 0, width, height };
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
			const int newWidth = rect.right - rect.left;
			const int newHeight = rect.bottom - rect.top;

			int xPos = GetSystemMetrics(SM_CXSCREEN) / 2 - newWidth / 2;
			int yPos = GetSystemMetrics(SM_CYSCREEN) / 2 - newHeight / 2;
			if (yPos < 0)
			{
				yPos = 0;
			}

			if ((m_hWnd = CreateWindowEx(0, pWindowName, pWindowName, WS_OVERLAPPEDWINDOW,
				xPos, yPos,	newWidth, newHeight, nullptr, nullptr, hInstance, pData)) != nullptr)
			{
				ShowWindow(m_hWnd, SW_SHOW);
			}
			else
			{
				result = GraphicsResult::WINDOW_FAIL;

				UnregisterClass(pWindowName, hInstance);
			}
		}

		return result;
	}


	void Window::Shutdown()
	{
		DestroyWindow(m_hWnd);

		char className[MAX_PATH];
		GetClassName(m_hWnd, className, MAX_PATH);
		UnregisterClass(className, nullptr);
	}


	HWND Window::GetHandle()
	{
		return m_hWnd;
	}
}