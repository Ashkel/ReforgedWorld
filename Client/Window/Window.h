#pragma once

#include <string>

class Container;

class Window
{
public:
	Window();
	virtual ~Window();

	bool Initialize(Container* pContainer, HINSTANCE hInstance, const TCHAR* title, const TCHAR* windowClass, int width, int height, bool isFullscreen = false);
	void Release();

	bool ProcessMessages();

	HWND GetWindowHandler() const;

private:
	bool RegisterWindowClass() const;

protected:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	DWORD m_dwWindowStyle;
	wchar_t m_szWindowClass[MAX_PATH];
};


