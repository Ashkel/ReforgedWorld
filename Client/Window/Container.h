#pragma once

#include "Window.h"

class Container
{
public:
	Container();
	virtual ~Container();

	LRESULT WINAPI WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	// Window Events
	virtual void OnActivated() = 0;
	virtual void OnDeactivated() = 0;
	virtual void OnSuspending() = 0;
	virtual void OnResuming() = 0;
	virtual void OnWindowMoved() = 0;
	virtual void OnDisplayChange() = 0;
	virtual void OnWindowSizeChanged(int width, int height) = 0;
	virtual void OnPaint() = 0;
	virtual void OnClosing() = 0;

protected:
	Window m_Window;

private:
	bool m_IsRawInputInitialized;
};

