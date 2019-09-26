#ifndef ENGINE_H
#define ENGINE_H

#include "Window.h"
#include "RendererD3D11.h"
#include "Timer.h"
#include "DeferredShading.h"

namespace BVEngine
{
#define MAXIMUM_NAME_SIZE 32
#define MINIMUM_WIDTH 200
#define MINIMUM_HEIGHT 200

	class Engine
	{
	public:
		Engine(const char * const pWindowName, const unsigned int width, const unsigned int height);
		virtual ~Engine();

		void Run();

	protected:
		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual bool StartUp();
		virtual void Shutdown();

		void Resize();
		void ToggleFullscreen(const bool value);

		// Inline methods
		inline void Stop() { m_IsRunning = false; }

		inline void IsPaused(const bool value) { m_IsPaused = value; }
		inline const bool IsPaused() const { return m_IsPaused; }

		inline void IsResizing(const bool value) { m_IsResizing = value; }
		inline const bool IsResizing() const { return m_IsResizing; }

	private:
		static LRESULT __stdcall StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		Engine() = delete;
		Engine(const Engine & other) = delete;
		Engine(Engine && other) = delete;
		Engine & operator = (const Engine & other) = delete;
		Engine & operator = (Engine && other) = delete;

	protected:
		Timer *m_pTimer;
		BVGraphics::Window *m_pWindow;
		BVGraphics::RendererD3D11 *m_pGraphics;
		DeferredShading *m_pDeferredShading;

		char m_Name[32];

		unsigned int m_Width;
		unsigned int m_Height;

		bool m_IsRunning;
		bool m_IsPaused;
		bool m_IsResizing;
		bool m_IsMaximized;
		bool m_IsMinimized;

		HANDLE m_hRenderThread;
		HANDLE m_hPauseEvent;
		bool m_PauseThread;
		static unsigned CALLBACK Thread(void *p);
	};
}
#endif