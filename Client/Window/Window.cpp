#include "pch.h"
#include "Container.h"
#include "Resource.h"

static LRESULT WINAPI HandleMsgRedirect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

		default:
		{
			Container* const pWindow = reinterpret_cast<Container*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

			if(pWindow)
				return pWindow->WindowProc(hWnd, message, wParam, lParam);
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		Container* pWindow = reinterpret_cast<Container*>(pCreate->lpCreateParams);

		if(pWindow == nullptr)
		{
			//LOGERROR("Critical Error: Pointer to window container is null during WM_NCCREATE!");

			exit(-1);

			return EXIT_FAILURE;
		}

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));

		return pWindow->WindowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



Window::Window()
	: m_hInstance(nullptr), m_hWnd(nullptr), m_dwWindowStyle(0u)
{
	::SetProcessDPIAware();
}

Window::~Window()
{
}

bool Window::Initialize(Container* pContainer, HINSTANCE hInstance, const TCHAR* title, const TCHAR* windowClass, int width, int height, bool isFullscreen)
{
	m_hInstance = hInstance;

#ifdef UNICODE
	wcscpy_s(m_szWindowClass, windowClass);
#else
	strcpy_s(m_szWindowClass, windowClass);
#endif // UNICODE

	if(!RegisterWindowClass())
	{
		//LOGERROR("RegisterWindowClass failed!\r\t-> Code: {0}", GetLastError());

		return false;
	}

	int x = CW_USEDEFAULT,
		y = CW_USEDEFAULT;

	if(isFullscreen)
	{
		m_dwWindowStyle = WS_POPUP | WS_VISIBLE;

		width = ::GetSystemMetrics(SM_CXSCREEN);
		height = ::GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		m_dwWindowStyle = WS_OVERLAPPEDWINDOW;
		RECT rc = { 0, 0, width, height };

		::AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);

		width = static_cast<int>(rc.right - rc.left);
		height = static_cast<int>(rc.bottom - rc.top);
		x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
		y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;
	}

	m_hWnd = ::CreateWindowEx(
		WS_EX_APPWINDOW,
		windowClass,
		title,
		m_dwWindowStyle,
		x,
		y,
		width,
		height,
		nullptr,
		nullptr,
		m_hInstance,
		pContainer);

	if(!m_hWnd)
	{
		//LOGERROR("CreateWindowEx failed!\r\t-> Code: {0}", GetLastError());

		return false;
	}

	Sleep(300);

	::ShowWindow(m_hWnd, SW_SHOW);
	::SetForegroundWindow(m_hWnd);
	::SetFocus(m_hWnd);
	::UpdateWindow(m_hWnd);

	return true;
}

void Window::Release()
{
	if(m_hWnd)
	{
		::DestroyWindow(m_hWnd);
		::UnregisterClass(m_szWindowClass, m_hInstance);
	}
}

bool Window::ProcessMessages()
{
	// Handle the windows messages
	MSG msg{};
	ZeroMemory(&msg, sizeof(MSG));

	if(::PeekMessage(&msg, m_hWnd, 0u, 0u, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Check if window was closed
	if(msg.message == WM_NULL)
	{
		if(!IsWindow(m_hWnd))
		{
			m_hWnd = nullptr;
			::UnregisterClass(m_szWindowClass, m_hInstance);

			return false;
		}
	}

	return true;
}

HWND Window::GetWindowHandler() const
{
	return m_hWnd;
}

bool Window::RegisterWindowClass() const
{
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc = HandleMsgSetup;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, TEXT("IDI_FAVICON"));
	wcex.hCursor = LoadCursor(m_hInstance, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = LoadIcon(m_hInstance, TEXT("IDI_FAVICON"));

	if(!::RegisterClassEx(&wcex))
		return false;

	return true;
}
