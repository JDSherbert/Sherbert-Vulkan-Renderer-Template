// Copyright (c) 2026 JDSherbert. All rights reserved.

#include "Window.h"

// Win32 window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_DESTROY:
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Create native Win32 window
Sherbert::Window Sherbert::CreateAppWindow(HINSTANCE hInstance, int width, int height)
{
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = ("SherbertWindowClass");

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        ("Sherbert Vulkan Window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr, nullptr, hInstance, nullptr
    );

    Window window{};
    window.handle = hwnd;
    window.width = width;
    window.height = height;

    return window;
}
