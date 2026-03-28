#pragma once

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <wchar.h>
#include <wrl/client.h>
#include <random>
#include "GameMesh.h"

//--------------------------------------------------------------------------------------
// 構造体
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT4 Color;
};

struct ConstantBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
    DirectX::XMVECTOR lightpos;
    DirectX::XMVECTOR eyepos;//★---追加---
};

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

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_D3DConstantBuffer;
    Microsoft::WRL::ComPtr<ID3DBlob> m_D3DBlobVS;
    DirectX::XMMATRIX m_matWorld;
    DirectX::XMMATRIX m_matView;
    DirectX::XMMATRIX m_matProjection;

    //------------------------------------------------------------
    // 球体の描画
    //------------------------------------------------------------
    const int m_iUMax = 48;
    const int m_iVMax = 24;
    int m_iVertexNum = m_iUMax * (m_iVMax + 1);
    int m_iIndexNum = 2 * m_iVMax * (m_iUMax + 1);

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