#pragma once

#include "Container.h"
#include <Core/StepTimer.h>

class Application : public Container
{
public:
	Application();
	virtual ~Application();

	bool Initialize(HINSTANCE hInstance, const TCHAR* title, const TCHAR* windowClass, int width, int height, bool isFullscreen = false);
	void Release();

	bool ProcessMessages();

	void Tick();

private:
	void ProcessInput();
	void Update(StepTimer const& timer);
	void Render();

	// Window Events
	// Inherited via ContainerWindow
	void OnActivated() override;

	void OnDeactivated() override;

	void OnSuspending() override;

	void OnResuming() override;

	void OnWindowMoved() override;

	void OnDisplayChange() override;

	void OnWindowSizeChanged(int width, int height) override;

	void OnPaint() override;

	void OnClosing() override;

private:
	// Update and Rendering loop timer.
	StepTimer m_timer;
};

