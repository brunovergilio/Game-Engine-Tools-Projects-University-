#ifndef WINDOW_H
#define WINDOW_H

#include "GraphicsUtils.h"

namespace BVGraphics
{
	class Window
	{
	public:
		Window();
		~Window();

		GraphicsResult Initialize(const char * const pWindowName, const int width, const int height, WNDPROC wndproc, void * const pData);
		void Shutdown();

		HWND GetHandle();

	private:
		Window(const Window & other) = delete;
		Window(Window && other) = delete;
		Window & operator = (const Window & other) = delete;
		Window & operator = (Window && other) = delete;

	private:
		HWND m_hWnd;
	};

}
#endif