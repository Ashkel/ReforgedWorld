#include "pch.h"
#include "Container.h"
#include "Input/Input.h"
#include <Core/SubsystemManager.hpp>

//#include "Shared/IO/Logger.h"

Container::Container()
	: m_Window(), m_IsRawInputInitialized(false)
{
	if(!m_IsRawInputInitialized)
	{
		RAWINPUTDEVICE rid{};

		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;

		if(RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			//LOGERROR("Failed to RegisterRawInputDevices!");

			exit(-1);
		}

		m_IsRawInputInitialized = true;
	}
}

Container::~Container()
{
}

#define GET_X_LPARAM(lp)	((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)	((int)(short)HIWORD(lp))

LRESULT WINAPI Container::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;

	switch(message)
	{
		case WM_PAINT:
		{
			if(s_in_sizemove)
				OnPaint();
			else
			{
				PAINTSTRUCT ps;
				std::ignore = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
			}

			break;
		}

		case WM_CLOSE:
		{
			OnClosing();

			// Releases with default WindowProc
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}

		case WM_DISPLAYCHANGE:
		{
			OnDisplayChange();
			break;
		}

		case WM_MOVE:
		{
			OnWindowMoved();
			break;
		}

		case WM_SIZE:
		{
			if(wParam == SIZE_MINIMIZED)
			{
				if(!s_minimized)
				{
					s_minimized = true;

					if(!s_in_suspend)
						OnSuspending();

					s_in_suspend = true;
				}
			}
			else if(s_minimized)
			{
				s_minimized = false;

				if(s_in_suspend)
					OnResuming();

				s_in_suspend = false;
			}
			else if(!s_in_sizemove)
				OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));

			break;
		}

		case WM_ENTERSIZEMOVE:
		{
			s_in_sizemove = true;
			break;
		}

		case WM_EXITSIZEMOVE:
		{
			s_in_sizemove = false;

			RECT rc{};
			::GetClientRect(hWnd, &rc);

			OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);

			break;
		}

		case WM_GETMINMAXINFO:
		{
			if(lParam)
			{
				auto info = reinterpret_cast<MINMAXINFO*>(lParam);
				info->ptMinTrackSize.x = 320;
				info->ptMinTrackSize.y = 200;
			}
			break;
		}

		case WM_ACTIVATEAPP:
		{
			if(wParam)
				OnActivated();
			else
				OnDeactivated();

			break;
		}

		case WM_POWERBROADCAST:
		{
			switch(wParam)
			{
				case PBT_APMQUERYSUSPEND:
				{
					if(!s_in_suspend)
						OnSuspending();

					s_in_suspend = true;

					return TRUE;
				}

				case PBT_APMRESUMESUSPEND:
				{
					if(!s_minimized)
					{
						if(s_in_suspend)
							OnResuming();

						s_in_suspend = false;
					}

					return TRUE;
				}
			}

			break;
		}

		case WM_MENUCHAR:
		{
			// A menu is active and the user presses a key that does not correspond
			// to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
			return MAKELRESULT(0, MNC_CLOSE);
		}

		// Keyboard messages
		case WM_KEYDOWN:
		{
			unsigned char key = static_cast<unsigned char>(wParam);

			auto& kbd = GetSubsystem<KeyboardQueue>();

			if(kbd.IsKeysAutoRepeat())
				kbd.OnKeyPressed(key);
			else
			{
				const bool wasPressed = lParam & 0x40000000;
				if(!wasPressed)
					kbd.OnKeyPressed(key);
			}

			break;
		}

		case WM_KEYUP:
		{
			unsigned char key = static_cast<unsigned char>(wParam);

			GetSubsystem<KeyboardQueue>().OnKeyReleased(key);

			break;
		}

		case WM_CHAR:
		{
			unsigned char ch = static_cast<unsigned char>(wParam);

			auto& kbd = GetSubsystem<KeyboardQueue>();

			if(kbd.IsCharsAutoRepeat())
				kbd.OnChar(ch);
			else
			{
				const bool wasPressed = lParam & 0x40000000;
				if(!wasPressed)
					kbd.OnChar(ch);
			}

			break;
		}

		// Mouse messages
		case WM_MOUSEMOVE:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnMouseMove(x, y);

			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnLeftPressed(x, y);

			break;
		}

		case WM_LBUTTONUP:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnLeftReleased(x, y);

			break;
		}

		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnRightPressed(x, y);

			break;
		}

		case WM_RBUTTONUP:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnRightReleased(x, y);

			break;
		}

		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnMiddlePressed(x, y);

			break;
		}

		case WM_MBUTTONUP:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			GetSubsystem<MouseQueue>().OnMiddleReleased(x, y);

			break;
		}

		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			WORD button = GET_XBUTTON_WPARAM(wParam);
			if(button == XBUTTON1)
				GetSubsystem<MouseQueue>().OnX1Pressed(x, y);
			else if(button == XBUTTON2)
				GetSubsystem<MouseQueue>().OnX2Pressed(x, y);

			break;
		}

		case WM_XBUTTONUP:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			WORD button = GET_XBUTTON_WPARAM(wParam);
			if(button == XBUTTON1)
				GetSubsystem<MouseQueue>().OnX1Released(x, y);
			else if(button == XBUTTON2)
				GetSubsystem<MouseQueue>().OnX2Released(x, y);

			break;
		}

		case WM_MOUSEWHEEL:
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if(wheelDelta > 0)
				GetSubsystem<MouseQueue>().OnWheelUp(x, y);
			else if(wheelDelta < 0)
				GetSubsystem<MouseQueue>().OnWheelDown(x, y);

			break;
		}

		case WM_MOUSEACTIVATE:
			// When you click activate the window, we want Mouse to ignore that event.
			return MA_ACTIVATEANDEAT;

		// Raw input
		case WM_INPUT:
		{
			if(m_IsRawInputInitialized)
			{
				HRAWINPUT hRawInput = reinterpret_cast<HRAWINPUT>(lParam);
				UINT dataSize = 0;

				GetRawInputData(hRawInput, RID_INPUT, nullptr, &dataSize, sizeof(RAWINPUTHEADER));

				if(dataSize > 0)
				{
					std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);

					UINT rawdataSize = GetRawInputData(hRawInput, RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER));

					if(rawdataSize == dataSize)
					{
						RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());

						if(raw->header.dwType == RIM_TYPEMOUSE)
						{
							GetSubsystem<MouseQueue>().OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
						}
					}
				}
			}

			break;
		}
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}
