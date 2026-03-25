#pragma once

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")//---★追加---
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include <d3d11_1.h>
#include <d3dcompiler.h>//---★追加---
#include <directxmath.h>//---★追加---
#include <directxcolors.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wchar.h>
#include <wrl/client.h>
#include <random>

//--------------------------★追加↓--------------------------
//--------------------------------------------------------------------------------------
// 構造体
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
};
//--------------------------★追加↑--------------------------

//--------------------------------------------------------------------------------------
// DirectX11クラス：DirectX関係
//--------------------------------------------------------------------------------------
class DirectX11
{
public:
    DirectX11();
    ~DirectX11();
    HRESULT CompileShaderFromFile(const WCHAR* wcFileName, LPCSTR lpEntryPoint, LPCSTR lpShaderModel, ID3DBlob** D3DBlob);//---★追加---
    HRESULT InitDevice();
    void Render();
private:
    //------------------------------------------------------------
    // DirectX11とDirect2D 1.1の初期化
    //------------------------------------------------------------
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3DDeviceContext;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_D2DDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_DXGISwapChain1;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_D2DBitmap1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_D3DRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_D3DVertexShader;//---★追加---
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_D3DPixelShader;//---★追加---
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_D3DVertexBuffer;//---★追加---

    //------------------------------------------------------------
    // DirectWriteの初期化
    //------------------------------------------------------------
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_DWriteTextFormat;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_D2DSolidBrush;
};

/*
    //マウス入力
    int iMouseL = 0, iMouseR = 0;
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) iMouseL = 1;
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) iMouseR = 1;
    //マウス座標取得
    POINT mousepoint;
    static FLOAT fMouseX, fMouseY;
    GetCursorPos(&mousepoint);//マウスのスクリーン座標取得
    ScreenToClient(Window::GethWnd(), &mousepoint);//スクリーン座標をクライアント座標（アプリケーションの左上を(0, 0)）に変換
    fMouseX = static_cast<FLOAT>(mousepoint.x);
    fMouseY = static_cast<FLOAT>(mousepoint.y);

    int iKeyA = 0, iKeyD = 0, iKeyW = 0, iKeyS = 0;
    if (GetAsyncKeyState('A') & 0x8000) iKeyA = 1;
    if (GetAsyncKeyState('D') & 0x8000) iKeyD = 1;
    if (GetAsyncKeyState('W') & 0x8000) iKeyW = 1;
    if (GetAsyncKeyState('S') & 0x8000) iKeyS = 1;
*/