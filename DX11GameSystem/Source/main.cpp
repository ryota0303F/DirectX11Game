#include "Main.h"
#include "DirectX.h"

HWND Window::g_hWnd = nullptr;
double Window::g_dFps = 0;
double Window::g_dFrameTime = 0;

//--------------------------------------------------------------------------------------
// 前方宣言
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    if (FAILED(CoInitialize(nullptr)))//COMの初期化
        return 0;

    Window win;

    if (FAILED(win.InitWindow(hInstance, nCmdShow)))
        return 0;

    DirectX11 dx;

    if (FAILED(dx.InitDevice()))
        return 0;

    win.InitFps();

    // メインメッセージループ
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            win.CalculationFps();

            win.CalculationFrameTime();

            dx.Render();

            win.CalculationSleep();
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT Window::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WindowClass";
    wcex.hIconSm = nullptr;
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"WindowClass", L"DirectXプログラミング",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

HWND Window::GethWnd()
{
    return g_hWnd;
}

//--------------------------★追加↓--------------------------
void Window::InitFps()
{

    QueryPerformanceFrequency(&Freq);
    QueryPerformanceCounter(&StartTime);//現在の時間を取得（1フレーム目）
}

void Window::CalculationFps()
{
    //FPSの計算
    if (iCount == 60)//カウントが60の時の処理
    {
        QueryPerformanceCounter(&NowTime);//現在の時間を取得（60フレーム目）
        //FPS = 1秒 / 1フレームの描画にかかる時間
        //    = 1000ms / ((現在の時間ms - 1フレーム目の時間ms) / 60フレーム)
        g_dFps = 1000.0 / (static_cast<double>((NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart) / 60.0);
        iCount = 0;//カウントを初期値に戻す
        StartTime = NowTime;//1フレーム目の時間を現在の時間にする
    }
    iCount++;//カウント+1
}

void Window::CalculationSleep()
{
    //Sleepさせる時間の計算
    QueryPerformanceCounter(&NowTime);//現在の時間を取得
    //Sleepさせる時間ms = 1フレーム目から現在のフレームまでの描画にかかるべき時間ms - 1フレーム目から現在のフレームまで実際にかかった時間ms
    //                  = (1000ms / 60)*フレーム数 - (現在の時間ms - 1フレーム目の時間ms)
    SleepTime = static_cast<DWORD>((1000.0 / 60.0) * iCount - (NowTime.QuadPart - StartTime.QuadPart) * 1000 / Freq.QuadPart);
    if (SleepTime > 0 && SleepTime < 18)//大きく変動がなければSleepTimeは1～17の間に納まる
    {
        timeBeginPeriod(1);
        Sleep(SleepTime);
        timeEndPeriod(1);
    }
    else//大きく変動があった場合
    {
        timeBeginPeriod(1);
        Sleep(1);
        timeEndPeriod(1);
    }
}

double Window::GetFps()
{
    return g_dFps;
}

void Window::CalculationFrameTime()
{
    //1フレームあたりの時間の計測
    static int a;
    if (a == 0)
    {
        QueryPerformanceCounter(&FrameTimeA);
        a = 1;
    }
    QueryPerformanceCounter(&FrameTimeB);
    g_dFrameTime = (FrameTimeB.QuadPart - FrameTimeA.QuadPart) * 1000.0 / Freq.QuadPart;
    FrameTimeA = FrameTimeB;
}

double Window::GetFrameTime()
{
    return g_dFrameTime;
}