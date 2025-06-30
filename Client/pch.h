// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0603
#endif
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

// Windows Header Files
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// DirectX Header Files
#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// C++ Header Files
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <typeinfo>
#include <functional>
#include <filesystem>

template<typename T>
concept HasRelease = requires(T t)
{
    { t.Release() } -> std::same_as<void>;
};

template<HasRelease T>
void SafeRelease(T*& ppT)
{
    if(ppT)
    {
        ppT->Release();
        ppT = nullptr;
    }
}

template<HasRelease T>
void SafeDelete(T*& ppT)
{
    delete ppT;
    ppT = nullptr;
}

template<HasRelease T>
void SafeDeleteArray(T*& ppT)
{
    delete[] ppT;
    ppT = nullptr;
}

