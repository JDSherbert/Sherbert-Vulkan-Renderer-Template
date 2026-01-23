// Copyright (c) 2026 JDSherbert. All rights reserved.

#pragma once

#include <windows.h>

#include "Renderer/Renderer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Sherbert::Window window = Sherbert::CreateAppWindow(hInstance, 800, 600);
    ShowWindow((HWND)window.handle, nCmdShow);

    Sherbert::Renderer renderer;
    renderer.Initialize(window);

    MSG msg{};
    bool running = true;
    while(running) {
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        renderer.DrawFrame();
    }

    renderer.WaitIdle();
    return 0;
}
