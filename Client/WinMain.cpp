// WinMain.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "Window/Application.h"

using namespace DirectX;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

LPCWSTR g_szAppName = L"Reforged World";

// Indicates to hybrid graphics systems to prefer the discrete part by default
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	AllocConsole();
#pragma warning(disable : 4996)
	(void)freopen("CONOUT$", "w", stdout);
#pragma warning restore
#endif // DEBUG

	try
	{
		if(!XMVerifyCPUSupport())
			return EXIT_FAILURE;

		HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		if(FAILED(hr))
			return EXIT_FAILURE;

		Application app;

		if(app.Initialize(hInstance, g_szAppName, g_szAppName, 800, 600, false))
		{
			while(app.ProcessMessages())
			{
				app.Tick();
			}

			app.Release();
		}

		CoUninitialize();
	}
	catch(const std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), "Error", MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}