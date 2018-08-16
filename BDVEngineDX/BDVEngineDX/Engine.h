#ifndef ENGINE_H
#define ENGINE_H

#include "Utils.h"
#include "Timer.h"

const int WINDOW_NAME_SIZE = 32;

class Engine
{
public:
	Engine(const HINSTANCE &hInstance, const char * const pWindowName, const UINT width = 640U, const UINT height = 480U);
	virtual ~Engine();

	void Run();

protected:
	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;

	// Windows Callback Procedure
	virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	// Not needed
	Engine() = delete;
	Engine(const Engine & engine) = delete;
	Engine & operator = (const Engine & engine) = delete;

protected:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	UINT m_Width;
	UINT m_Height;

	bool m_IsRunning;
	bool m_IsPaused;

	Timer m_Timer;

	char m_pWindowName[WINDOW_NAME_SIZE];
};

#endif