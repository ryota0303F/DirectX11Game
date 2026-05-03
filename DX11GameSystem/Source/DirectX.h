#pragma once

#include <dwrite.h>
#include <wchar.h>
#include <wrl/client.h>
#include <random>
#include "GameMesh.h"
#include "GraphicsTypes.h"


//--------------------------------------------------------------------------------------
// DirectX11クラス：DirectX関係
//--------------------------------------------------------------------------------------
class DirectX11
{
public:
    static DirectX11& Instance();
    ~DirectX11();
    HRESULT CompileShaderFromFile(const WCHAR* wcFileName, LPCSTR lpEntryPoint, LPCSTR lpShaderModel, ID3DBlob** D3DBlob);
    HRESULT InitDevice();
    void Render();

    ID3D11DeviceContext* GetDeviceContext3D();
    ID3D11Device* GetDevice3D();
    ID3DBlob* GetVertexShaderBlob();
    ID3D11VertexShader* GetVertexShader();
    ID3D11PixelShader* GetPixelShader();
    ID3D11Buffer** GetConstantBuffer();
private:
    DirectX11();

    //------------------------------------------------------------
    // DirectX11とDirect2D 1.1の初期化
    //------------------------------------------------------------
    Microsoft::WRL::ComPtr<ID3D11Device> m_D3DDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3DDeviceContext;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_D2DDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> m_DXGISwapChain1;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_D2DBitmap1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_D3DRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_D3DVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_D3DPixelShader;

    //Microsoft::WRL::ComPtr<ID3D11Buffer> m_D3DConstantBuffer;
    Microsoft::WRL::ComPtr<ID3DBlob> m_D3DBlobVS;

    //------------------------------------------------------------
    // DirectWriteの初期化
    //------------------------------------------------------------
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_DWriteTextFormat;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_D2DSolidBrush;

    GameMesh mGameMesh1;
    GameMesh mGameMesh2;
};

#define DX11 DirectX11::Instance()

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