#include "Engine.h"
#include "MemoryHelpers.h"
#include <process.h>

namespace BVEngine
{
	Engine::Engine(const char * const pWindowName, const unsigned int width, const unsigned int height)
		: m_Width(width), m_Height(height), m_pWindow(nullptr), m_pGraphics(nullptr), m_pDeferredShading(nullptr), m_pTimer(nullptr),
		m_IsRunning(false), m_IsPaused(false), m_IsResizing(false), m_IsMaximized(false), m_IsMinimized(false),
		m_PauseThread(false), m_hRenderThread(nullptr), m_hPauseEvent(nullptr)
	{
		strcpy_s(m_Name, MAXIMUM_NAME_SIZE, pWindowName);
	}


	Engine::~Engine()
	{
		Shutdown();
	}


	void Engine::Run()
	{
		bool startUpOK = false;
		if (StartUp())
		{
			m_IsRunning = true;
			startUpOK = true;

			// Thread test
			ResumeThread(m_hRenderThread);
			m_PauseThread = false;
			// Thread test
		}

		MSG msg;
		while (m_IsRunning)
		{
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			//if (!m_IsPaused)
			//{
			//	m_pTimer->Update();
			//	Update();
			//	Render();
			//}
		}

		if (startUpOK)
		{
			// Thread test
			WaitForSingleObject(m_hRenderThread, INFINITE);
			// Thread test

			Shutdown();
		}
	}


	bool Engine::StartUp()
	{
		Shutdown();

		bool result = true;

		do
		{
			m_pWindow = BV_NEW(BVGraphics::Window, nullptr) Window();
			if (GraphicsResult::WINDOW_FAIL == m_pWindow->Initialize(m_Name, m_Width, m_Height, StaticWndProc, this))
			{
				result = false;
				break;
			}

			m_pGraphics = BV_NEW(BVGraphics::RendererD3D11, nullptr) RendererD3D11();
			if (GraphicsResult::GRAPHICS_FAIL == m_pGraphics->Initialize(m_pWindow->GetHandle(), m_Width, m_Height))
			{
				result = false;
				break;
			}

			m_pDeferredShading = BV_NEW(DeferredShading, nullptr) DeferredShading();
			if (GraphicsResult::SUCCESS != m_pDeferredShading->Create(m_Width, m_Height))
			{
				result = false;
				break;
			}

			m_pTimer = BV_NEW(Timer, nullptr) Timer();

			// Thread test
			m_hRenderThread = reinterpret_cast<HANDLE>(
				_beginthreadex(nullptr, 0U, Engine::Thread, this, CREATE_SUSPENDED, nullptr)
				);
			m_hPauseEvent = CreateEvent(nullptr, true, false, nullptr);
			// Thread test
		} while (0);

		return result;
	}


	void Engine::Shutdown()
	{
		if (m_pDeferredShading)
		{
			BV_DELETE(m_pDeferredShading, nullptr);
			m_pDeferredShading = nullptr;
		}

		if (m_pGraphics)
		{
			m_pGraphics->Shutdown();
			BV_DELETE(m_pGraphics, nullptr);
			m_pGraphics = nullptr;
		}

		if (m_pWindow)
		{
			m_pWindow->Shutdown();
			BV_DELETE(m_pWindow, nullptr);
			m_pWindow = nullptr;
		}

		if (m_pTimer)
		{
			BV_DELETE(m_pTimer, nullptr);
			m_pTimer = nullptr;
		}

		// Thread test
		if (m_hRenderThread)
		{
			CloseHandle(m_hRenderThread);
			m_hRenderThread = nullptr;
		}
		if (m_hPauseEvent)
		{
			CloseHandle(m_hPauseEvent);
			m_hPauseEvent = nullptr;
		}
		// Thread test
	}


	void Engine::Resize()
	{
		if (m_pGraphics)
		{
			m_pGraphics->Resize(m_Width, m_Height);

			m_pDeferredShading->ResizeGBuffer(m_Width, m_Height);
		}

		// Thread test
		m_PauseThread = false;
		ResetEvent(m_hPauseEvent);
		// Thread test
	}


	void Engine::ToggleFullscreen(const bool value)
	{
		if (m_pGraphics)
		{
			m_pGraphics->ToggleFullScreen(value);
		}
	}


	LRESULT Engine::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Engine *pEngine = nullptr;

		if (msg == WM_NCCREATE)
		{
			pEngine = reinterpret_cast<Engine*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(pEngine));
		}
		else
		{
			pEngine = reinterpret_cast<Engine*>(GetWindowLong(hWnd, GWL_USERDATA));

			switch (msg)
			{
			case WM_ACTIVATE:
				if (LOWORD(wParam) == WA_INACTIVE)
				{
					pEngine->m_IsPaused = true;
				}
				else
				{
					pEngine->m_IsPaused = false;
				}
				return 0;

			case WM_SIZE:
				{
					// Thread test
					if (!pEngine->m_IsResizing)
					{
						pEngine->m_PauseThread = true;
						WaitForSingleObject(pEngine->m_hPauseEvent, INFINITE);
					}
					// Thread test
					pEngine->m_Width = LOWORD(lParam);
					pEngine->m_Height = HIWORD(lParam);
					if (wParam == SIZE_MINIMIZED)
					{
						pEngine->m_IsMinimized = true;
						pEngine->m_IsMaximized = false;
					}
					else if (wParam == SIZE_MAXIMIZED)
					{
						pEngine->Resize();
						pEngine->m_IsPaused = false;
						pEngine->m_IsMinimized = false;
						pEngine->m_IsMaximized = true;
					}
					else if (wParam == SIZE_RESTORED)
					{
						if (pEngine->m_IsMinimized)
						{
							pEngine->Resize();
							pEngine->m_IsPaused = false;
							pEngine->m_IsMinimized = false;
						}
						else if (pEngine->m_IsMaximized)
						{
							pEngine->Resize();
							pEngine->m_IsPaused = false;
							pEngine->m_IsMaximized = false;
						}
					}
				}
				return 0;

			case WM_ENTERSIZEMOVE:
				pEngine->m_IsPaused = true;
				pEngine->m_IsResizing = true;
				return 0;

			case WM_EXITSIZEMOVE:
				pEngine->Resize();
				pEngine->m_IsPaused = false;
				pEngine->m_IsResizing = false;
				return 0;

			case WM_GETMINMAXINFO:
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = MINIMUM_WIDTH;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = MINIMUM_HEIGHT;
				return 0;

			case WM_CLOSE:
				pEngine->Stop();
				return 0;

			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}


	unsigned Engine::Thread(void * p)
	{
		Engine *pEngine = reinterpret_cast<Engine *>(p);
		while (pEngine->m_IsRunning)
		{
			if (pEngine->m_PauseThread && !pEngine->m_IsPaused)
			{
				pEngine->m_IsPaused = true;
				SetEvent(pEngine->m_hPauseEvent);
			}

			if (!pEngine->m_IsPaused)
			{
				pEngine->m_pTimer->Update();
				pEngine->Update();
				pEngine->Render();
			}
		}

		return 0;
	}
}