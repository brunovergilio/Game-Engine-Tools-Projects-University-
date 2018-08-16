#include "Engine.h"


Engine::Engine(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width, const UINT height)
	: m_hInstance(hInstance), m_Width(width), m_Height(height), m_IsRunning(false), m_IsPaused(false)
{
	assert(strlen(pWindowName) < 32);
	
	strcpy_s(m_pWindowName, pWindowName);
}

Engine::~Engine()
{
}

void Engine::Run()
{
	if (!Initialize())
	{
		return;
	}

	m_Timer.Start();

	MSG msg = { 0 };
	while (m_IsRunning)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!m_IsPaused)
		{
			m_Timer.Update();
			Update();
			Render();
		}
	}

	Shutdown();
}

LRESULT CALLBACK Engine::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Engine *engine = nullptr;

	if (msg == WM_NCCREATE)
	{
		engine = reinterpret_cast<Engine*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
		SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(engine));
	}
	else
	{
		engine = reinterpret_cast<Engine*>(GetWindowLong(hWnd, GWL_USERDATA));
	}

	return engine ? engine->WndProc(hWnd, msg, wParam, lParam) : DefWindowProc(hWnd, msg, wParam, lParam);
}