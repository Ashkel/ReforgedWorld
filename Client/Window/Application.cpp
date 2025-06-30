#include "pch.h"
#include "Application.h"

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
}

void Application::Update(StepTimer const& timer)
{
}

void Application::Render()
{
}

void Application::OnActivated()
{
	std::string out = "OnActivated -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnDeactivated()
{
	std::string out = "OnDeactivated -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnSuspending()
{
	std::string out = "OnSuspending -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnResuming()
{
	std::string out = "OnResuming -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnWindowMoved()
{
	std::string out = "OnWindowMoved -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnDisplayChange()
{
	std::string out = "OnDisplayChange -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnWindowSizeChanged(int width, int height)
{
	std::string out = "OnWindowSizeChanged -> ";
	out += "Width: " + std::to_string(width);
	out += ", Height: " + std::to_string(height);
	out += "\n";
	OutputDebugStringA(out.c_str());
}

void Application::OnPaint()
{
	std::string out = "OnPaint -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());

	Tick();
}

void Application::OnClosing()
{
	std::string out = "OnClosing -> ";
	out += "\n";
	OutputDebugStringA(out.c_str());
}
