#pragma once

#include <windows.h>
#pragma comment(lib,"winmm.lib")

class Window
{
public:
    HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
    void InitFps();
    void CalculationFps();
    void CalculationSleep();
    void CalculationFrameTime();
    static HWND GethWnd();
    static double GetFps();
    static double GetFrameTime();
private:
    static HWND g_hWnd;
    static double g_dFps;
    static double g_dFrameTime;
    LARGE_INTEGER Freq = { 0 };
    LARGE_INTEGER StartTime = { 0 };
    LARGE_INTEGER NowTime = { 0 };
    LARGE_INTEGER FrameTimeA = { 0 };
    LARGE_INTEGER FrameTimeB = { 0 };
    int iCount = 0;
    DWORD SleepTime = 0;
};