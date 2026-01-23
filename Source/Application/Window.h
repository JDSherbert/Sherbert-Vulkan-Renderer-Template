// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <cstdint>
#include <windows.h>

namespace Sherbert {

    // Minimal window wrapper for Win32
    struct Window
    {
        void* handle;      // HWND
        uint32_t width;
        uint32_t height;
    };

    // Creates a Win32 window and returns a NativeWindow struct
    Window CreateAppWindow(HINSTANCE hInstance, int width, int height);

}
