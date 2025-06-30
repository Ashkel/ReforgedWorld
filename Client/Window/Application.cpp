#include "pch.h"
#include "Application.h"
#include "Input/Input.h"

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Initialize(HINSTANCE hInstance, const TCHAR* title, const TCHAR* windowClass, int width, int height, bool isFullscreen)
{
	bool result = m_Window.Initialize(this, hInstance, title, windowClass, width, height, isFullscreen);
	if(!result)
		return false;

	return true;
}

void Application::Release()
{
	m_Window.Release();
}

bool Application::ProcessMessages()
{
	return m_Window.ProcessMessages();
}

void Application::Tick()
{
	ProcessInput();

	m_timer.Tick([&]()
				 {
					 Update(m_timer);
				 });

	Render();
}

void Application::ProcessInput()
{
	/*auto& kbd = GetSubsystem<KeyboardQueue>();

	if(!kbd.CharBufferIsEmpty())
	{
		unsigned char ch = kbd.ReadChar();

		std::string out = "OnChar: ";
		out += ch;
		out += "\n";

		OutputDebugStringA(out.c_str());
	}

	if(!kbd.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = kbd.ReadKey();
		unsigned char keyCode = kbe.GetKeyCode();

		std::string out = "";

		if(kbe.IsPress())
			out += "OnKeyDown";
		else if(kbe.IsRelease())
			out += "OnKeyUp";

		out += " -> KeyCode: ";
		out += keyCode;
		out += "\n";

		OutputDebugStringA(out.c_str());
	}*/

	/*auto& mouse = GetSubsystem<MouseQueue>();
	if(!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();

		std::string out = "";

		if(me.GetType() == MouseEvent::LeftPress)
			out += "LeftDown";
		else if(me.GetType() == MouseEvent::LeftRelease)
			out += "LeftUp";
		if(me.GetType() == MouseEvent::RightPress)
			out += "RightPress";
		else if(me.GetType() == MouseEvent::RightRelease)
			out += "RightRelease";
		else
		{
			if(me.GetType() == MouseEvent::Move)
			{
				out += "Move -> X: " + std::to_string(me.GetX());
				out += ", Y: " + std::to_string(me.GetY());
			}

			if(me.GetType() == MouseEvent::RawMove)
			{
				out += "RawMove -> X: " + std::to_string(me.GetX());
				out += ", Y: " + std::to_string(me.GetY());
			}
		}

		if(mouse.IsDoubleClick())
		{
			if(me.GetType() == mouse.GetDoubleClickButton())
				out += " Double Clicked";
		}

		out += "\n";

		OutputDebugStringA(out.c_str());
	}*/
}

void Application::Update(StepTimer const& timer)
{
}

void Application::Render()
{
}

void Application::OnActivated()
{
	/*std::string out = "OnActivated -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnDeactivated()
{
	/*std::string out = "OnDeactivated -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnSuspending()
{
	/*std::string out = "OnSuspending -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnResuming()
{
	/*std::string out = "OnResuming -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnWindowMoved()
{
	/*std::string out = "OnWindowMoved -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnDisplayChange()
{
	/*std::string out = "OnDisplayChange -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnWindowSizeChanged(int width, int height)
{
	/*std::string out = "OnWindowSizeChanged -> ";
	out += "Width: " + std::to_string(width);
	out += ", Height: " + std::to_string(height);
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}

void Application::OnPaint()
{
	/*std::string out = "OnPaint -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/

	Tick();
}

void Application::OnClosing()
{
	/*std::string out = "OnClosing -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());*/
}
