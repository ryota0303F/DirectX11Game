#include "Main.h"
#include "DirectX.h"

//--------------------------------------------------------------------------------------
// DirectX11::DirectX11()関数：コンストラクタ
//--------------------------------------------------------------------------------------
DirectX11::DirectX11()
{
    m_matWorld = DirectX::XMMatrixIdentity();
    m_matView = DirectX::XMMatrixIdentity();
    m_matProjection = DirectX::XMMatrixIdentity();
}

DirectX11& DirectX11::Instance() {
    //static変数は一度だけ初期化される
    static DirectX11 instance;
    return instance;
}

//--------------------------------------------------------------------------------------
// DirectX11::~DirectX11関数：デストラクタ
//--------------------------------------------------------------------------------------
DirectX11::~DirectX11()
{

}

//--------------------------------------------------------------------------------------
// DirectX11::CompileShaderFromFile()：シェーダのコンパイル
//--------------------------------------------------------------------------------------
HRESULT DirectX11::CompileShaderFromFile(const WCHAR* wcFileName, LPCSTR lpEntryPoint, LPCSTR lpShaderModel, ID3DBlob** D3DBlob)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> D3DErrorBlob;
    hr = D3DCompileFromFile(wcFileName, nullptr, nullptr, lpEntryPoint, lpShaderModel,
        dwShaderFlags, 0, D3DBlob, &D3DErrorBlob);
    if (FAILED(hr))
    {
        if (D3DErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(D3DErrorBlob->GetBufferPointer()));
        }
        return hr;
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------
// DirectX11::InitDevice()：DirectX関係の初期化
//--------------------------------------------------------------------------------------
HRESULT DirectX11::InitDevice()
{
    //------------------------------------------------------------
    // DirectX11とDirect2D 1.1の初期化
    //------------------------------------------------------------
    HRESULT hr = S_OK;

    UINT uiDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;//DirectX11上でDirect2Dを使用するために必要
#ifdef _DEBUG
    uiDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE drivertypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT uiDriverTypesNum = ARRAYSIZE(drivertypes);

    D3D_FEATURE_LEVEL featurelevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT uiFeatureLevelsNum = ARRAYSIZE(featurelevels);

    D3D_DRIVER_TYPE drivertype = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featurelevel = D3D_FEATURE_LEVEL_11_0;
    for (UINT uiDriverTypeIndex = 0; uiDriverTypeIndex < uiDriverTypesNum; uiDriverTypeIndex++)
    {
        drivertype = drivertypes[uiDriverTypeIndex];
        hr = D3D11CreateDevice(nullptr, drivertype, nullptr, uiDeviceFlags, featurelevels, uiFeatureLevelsNum,
            D3D11_SDK_VERSION, &m_D3DDevice, &featurelevel, &m_D3DDeviceContext);//&D3DDevice &m_D3DDeviceContext 初期化

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, drivertype, nullptr, uiDeviceFlags, &featurelevels[1], uiFeatureLevelsNum - 1,
                D3D11_SDK_VERSION, &m_D3DDevice, &featurelevel, &m_D3DDeviceContext);//&D3DDevice &m_D3DDeviceContext 初期化
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IDXGIDevice2> DXGIDevice2;
    hr = m_D3DDevice.As(&DXGIDevice2);//D3DDevice->QueryInterface()ではなくD3DDevice.As()、&DXGIDevice2 初期化
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<ID2D1Factory1> D2DFactory1;
    D2D1_FACTORY_OPTIONS factoryoptions = {};
#ifdef _DEBUG
    factoryoptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factoryoptions, D2DFactory1.GetAddressOf());//&D2DFactory1ではなくD2DFactory1.GetAddressOf()
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<ID2D1Device> D2D1Device;
    hr = D2DFactory1->CreateDevice(DXGIDevice2.Get(), &D2D1Device);//DXGIDevice2ではなくDXGIDevice2.Get()、&D2D1Device 初期化
    if (FAILED(hr))
        return hr;

    hr = D2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DDeviceContext);//&m_D2DDeviceContext 初期化
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IDXGIAdapter> DXGIAdapter;
    hr = DXGIDevice2->GetAdapter(&DXGIAdapter);//&DXGIAdapter 初期化
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIFactory2;
    hr = DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory2));//&DXGIFactory2 初期化
    if (FAILED(hr))
        return hr;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = Window::GetClientWidth();
    desc.Height = Window::GetClientHeight();
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    hr = DXGIFactory2->CreateSwapChainForHwnd(m_D3DDevice.Get(), Window::GethWnd(), &desc, nullptr, nullptr, &m_DXGISwapChain1);//D3DDeviceではなくD3DDevice.Get()、&m_DXGISwapChain1 初期化
    if (FAILED(hr))
        return hr;

    (void)DXGIDevice2->SetMaximumFrameLatency(1);

    DXGIFactory2->MakeWindowAssociation(Window::GethWnd(), DXGI_MWA_NO_ALT_ENTER);//Alt+Enter時フルスクリーンを無効

    Microsoft::WRL::ComPtr<IDXGISurface2> DXGISurface2;
    hr = m_DXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&DXGISurface2));//&DXGISurface2 初期化
    if (FAILED(hr))
        return hr;

    hr = m_D2DDeviceContext->CreateBitmapFromDxgiSurface(DXGISurface2.Get(),//DXGISurface2ではなくDXGISurface2.Get()
        D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &m_D2DBitmap1);//&m_D2DBitmap1 初期化
    if (FAILED(hr))
        return hr;

    m_D2DDeviceContext->SetTarget(m_D2DBitmap1.Get());//D2DBitmap1ではなくD2DBitmap1.Get()

    Microsoft::WRL::ComPtr<ID3D11Texture2D> D3DTexture2D;
    hr = m_DXGISwapChain1->GetBuffer(0, IID_PPV_ARGS(&D3DTexture2D));//&D3DTexture2D 初期化
    if (FAILED(hr))
        return hr;

    hr = m_D3DDevice->CreateRenderTargetView(D3DTexture2D.Get(), nullptr, &m_D3DRenderTargetView);//D3DTexture2DではなくD3DTexture2D.Get()、&m_D3DRenderTargetView 初期化
    if (FAILED(hr))
        return hr;

    m_D3DDeviceContext->OMSetRenderTargets(1, m_D3DRenderTargetView.GetAddressOf(), nullptr);//&m_D3DRenderTargetViewではなくm_D3DRenderTargetView.GetAddressOf()

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)Window::GetClientWidth();
    vp.Height = (FLOAT)Window::GetClientHeight();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_D3DDeviceContext->RSSetViewports(1, &vp);

    //バーテックスシェーダのコンパイル
    hr = CompileShaderFromFile(L"Shader/VertexShader.hlsl", "main", "vs_5_0", &m_D3DBlobVS);
    if (FAILED(hr))
        return hr;

    //バーテックスシェーダの作成
    hr = m_D3DDevice->CreateVertexShader(m_D3DBlobVS->GetBufferPointer(), m_D3DBlobVS->GetBufferSize(), nullptr, &m_D3DVertexShader);
    if (FAILED(hr))
        return hr;

    //インプットレイアウトの定義
    //D3D11_INPUT_ELEMENT_DESC layout[] =
    //{
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    //5番目のパラメータ：先頭からのバイト数4バイト(FLAOT)×3(RGB)=12
    //    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //};
    //UINT uiElements = ARRAYSIZE(layout);

    ////インプットレイアウトの作成
    //Microsoft::WRL::ComPtr<ID3D11InputLayout> D3DInputLayout;
    //hr = m_D3DDevice->CreateInputLayout(layout, uiElements, D3DBlobVS->GetBufferPointer(),
    //    D3DBlobVS->GetBufferSize(), &D3DInputLayout);
    //if (FAILED(hr))
    //    return hr;

    //インプットレイアウトの設定
    //m_D3DDeviceContext->IASetInputLayout(D3DInputLayout.Get());

    //ピクセルシェーダのコンパイル
    Microsoft::WRL::ComPtr<ID3DBlob> D3DBlobPS;
    hr = CompileShaderFromFile(L"Shader/PixelShader.hlsl", "main", "ps_5_0", &D3DBlobPS);
    if (FAILED(hr))
        return hr;

    //ピクセルシェーダの作成
    hr = m_D3DDevice->CreatePixelShader(D3DBlobPS->GetBufferPointer(), D3DBlobPS->GetBufferSize(), nullptr, &m_D3DPixelShader);
    if (FAILED(hr))
        return hr;

    //バーテックスバッファの作成
    //std::vector<SimpleVertex> vertices(m_iVertexNum);
    //for (int v = 0; v <= m_iVMax; v++)
    //{
    //    for (int u = 0; u < m_iUMax; u++)
    //    {
    //        double dTheta = DirectX::XMConvertToRadians(180.0f * v / m_iVMax);
    //        double dPhi = DirectX::XMConvertToRadians(360.0f * u / m_iUMax);
    //        FLOAT fX = static_cast<FLOAT>(sin(dTheta) * cos(dPhi));
    //        FLOAT fY = static_cast<FLOAT>(cos(dTheta));
    //        FLOAT fZ = static_cast<FLOAT>(sin(dTheta) * sin(dPhi));
    //        vertices[m_iUMax * v + u].Pos = DirectX::XMFLOAT3(fX, fY, fZ);
    //        vertices[m_iUMax * v + u].Normal = DirectX::XMFLOAT3(fX, fY, fZ);
    //    }
    //}
    //D3D11_BUFFER_DESC bd = {};
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.ByteWidth = sizeof(SimpleVertex) * m_iVertexNum;
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    //D3D11_SUBRESOURCE_DATA sub = {};
    //sub.pSysMem = &vertices.front();
    //hr = m_D3DDevice->CreateBuffer(&bd, &sub, &m_D3DVertexBuffer);
    //if (FAILED(hr))
    //    return hr;

    ////バーテックスバッファの設定
    //UINT uiStride = sizeof(SimpleVertex);
    //UINT uiOffset = 0;
    //m_D3DDeviceContext->IASetVertexBuffers(0, 1, m_D3DVertexBuffer.GetAddressOf(), &uiStride, &uiOffset);

    ////インデックスバッファの作成
    //Microsoft::WRL::ComPtr<ID3D11Buffer> D3DIndexBuffer;
    //int i = 0;
    //std::vector<WORD> indices(m_iIndexNum);
    //for (int v = 0; v < m_iVMax; v++)
    //{
    //    for (int u = 0; u <= m_iUMax; u++)
    //    {
    //        if (u == m_iUMax)
    //        {
    //            indices[i] = v * m_iUMax;
    //            i++;
    //            indices[i] = (v + 1) * m_iUMax;
    //            i++;
    //        }
    //        else
    //        {
    //            indices[i] = (v * m_iUMax) + u;
    //            i++;
    //            indices[i] = indices[i - 1] + m_iUMax;
    //            i++;
    //        }
    //    }
    //}
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.ByteWidth = sizeof(WORD) * m_iIndexNum;
    //bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //bd.CPUAccessFlags = 0;
    //sub.pSysMem = &indices.front();
    //hr = m_D3DDevice->CreateBuffer(&bd, &sub, &D3DIndexBuffer);
    //if (FAILED(hr))
    //    return hr;

    ////インデックスバッファの設定
    //m_D3DDeviceContext->IASetIndexBuffer(D3DIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    //プリミティブトポロジの設定
    m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //コンスタントバッファの作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(SimpleVertex) * m_iVertexNum;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = m_D3DDevice->CreateBuffer(&bd, nullptr, &m_D3DConstantBuffer);
    if (FAILED(hr))
        return hr;

    ////ラスタライザの作成
    //Microsoft::WRL::ComPtr<ID3D11RasterizerState> D3DRasterizerState;
    //D3D11_RASTERIZER_DESC ras = {};
    ////D3D11_FILL_WIREFRAME（ワイヤーフレーム） D3D11_FILL_SOLID（ソリッド）
    //ras.FillMode = D3D11_FILL_SOLID;
    ////D3D11_CULL_NONE（カリングなし：裏表描画） D3D11_CULL_FRONT（表面カリング：裏面描画） D3D11_CULL_BACK（裏面カリング：表面描画）
    //ras.CullMode = D3D11_CULL_BACK;
    //ras.FrontCounterClockwise = TRUE;
    //hr = m_D3DDevice->CreateRasterizerState(&ras, &D3DRasterizerState);
    //if (FAILED(hr))
    //    return hr;

    //ラスタライザの設定
    //m_D3DDeviceContext->RSSetState(D3DRasterizerState.Get());

    //ワールドマトリックスの設定
    m_matWorld = DirectX::XMMatrixIdentity();

    //プロジェクションマトリックスの設定
    m_matProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, static_cast<FLOAT>(Window::GetClientWidth()) / static_cast<FLOAT>(Window::GetClientHeight()), 0.01f, 100.0f);//★---変更---

    //シェーダのセット
    //m_D3DDeviceContext->VSSetShader(m_D3DVertexShader.Get(), nullptr, 0);
    //m_D3DDeviceContext->VSSetConstantBuffers(0, 1, m_D3DConstantBuffer.GetAddressOf());
    //m_D3DDeviceContext->PSSetShader(m_D3DPixelShader.Get(), nullptr, 0);
    //m_D3DDeviceContext->PSSetConstantBuffers(0, 1, m_D3DConstantBuffer.GetAddressOf());

    mGameMesh1.Load();
    mGameMesh2.Load();

    //------------------------------------------------------------
    // DirectWriteの初期化
    //------------------------------------------------------------
    Microsoft::WRL::ComPtr<IDWriteFactory> DWriteFactory;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &DWriteFactory);//&DWriteFactory 初期化
    if (FAILED(hr))
        return hr;

    //関数CreateTextFormat()
    //第1引数：フォント名（L"メイリオ", L"Arial", L"Meiryo UI"等）
    //第2引数：フォントコレクション（nullptr）
    //第3引数：フォントの太さ（DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_WEIGHT_BOLD等）
    //第4引数：フォントスタイル（DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STYLE_ITALIC）
    //第5引数：フォントの幅（DWRITE_FONT_STRETCH_NORMAL,DWRITE_FONT_STRETCH_EXTRA_EXPANDED等）
    //第6引数：フォントサイズ（20, 30等）
    //第7引数：ロケール名（L""）
    //第8引数：テキストフォーマット（&g_pTextFormat）
    hr = DWriteFactory->CreateTextFormat(L"メイリオ", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"", &m_DWriteTextFormat);//&m_DWriteTextFormat 初期化
    if (FAILED(hr))
        return hr;

    //関数SetTextAlignment()
    //第1引数：テキストの配置（DWRITE_TEXT_ALIGNMENT_LEADING：前, DWRITE_TEXT_ALIGNMENT_TRAILING：後, DWRITE_TEXT_ALIGNMENT_CENTER：中央）
    hr = m_DWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if (FAILED(hr))
        return hr;

    //関数CreateSolidColorBrush()
    //第1引数：フォント色（D2D1::ColorF(D2D1::ColorF::Black)：黒, D2D1::ColorF(D2D1::ColorF(0.0f, 0.2f, 0.9f, 1.0f))：RGBA指定）
    hr = m_D2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_D2DSolidBrush);//&m_D2DSolidBrush 初期化
    if (FAILED(hr))
        return hr;

    return S_OK;
}

//--------------------------------------------------------------------------------------
// DirectX11::Render()：DirectX関係の描画
//--------------------------------------------------------------------------------------
void DirectX11::Render()
{
    m_D3DDeviceContext->ClearRenderTargetView(m_D3DRenderTargetView.Get(), DirectX::Colors::Aquamarine);//m_D3DRenderTargetViewではなくm_D3DRenderTargetView.Get()

    //------------------------------------------------------------
    // 初期設定
    //------------------------------------------------------------
    static FLOAT fTheta = 5.0f;//カメラ横方向角度//★---変更---
    static FLOAT fDelta = -0.06f;//カメラ縦方向角度//★---変更---
    static bool bMouseR_drag;//マウス右ドラッグフラグ
    static FLOAT fDistance = 4;//カメラ位置から焦点までの距離
    static POINT mousepoint_a;//マウス位置
    static POINT mousepoint_b;//マウス位置
    static FLOAT fCameraX = fDistance * cos(fDelta) * cos(fTheta);//カメラの位置X座標
    static FLOAT fCameraY = fDistance * sin(fDelta);//カメラの位置Y座標
    static FLOAT fCameraZ = fDistance * cos(fDelta) * sin(fTheta);//カメラの位置Z座標

    //------------------------------------------------------------
    // キー入力関係
    //------------------------------------------------------------
    //マウス右クリック
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && bMouseR_drag == false && Window::GetWindowActive() == true)
    {
        bMouseR_drag = true;//マウス右ドラッグフラグ

        GetCursorPos(&mousepoint_a);//マウスのスクリーン座標取得
    }
    else if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
    {
        bMouseR_drag = false;//マウス右ドラッグフラグ
    }
    //マウス右ドラッグ
    if (bMouseR_drag)
    {
        GetCursorPos(&mousepoint_b);//マウスのスクリーン座標取得

        fTheta -= (mousepoint_b.x - mousepoint_a.x) * 0.003f;//カメラ横方向角度変更

        if (fDelta + (mousepoint_b.y - mousepoint_a.y) * 0.003f >= DirectX::XM_PI / 2.0f - 0.0001f)
        {
            fDelta = DirectX::XM_PI / 2.0f - 0.0001f;//カメラ縦方向角度変更
        }
        else if (fDelta + (mousepoint_b.y - mousepoint_a.y) * 0.003f <= -DirectX::XM_PI / 2.0f + 0.0001f)
        {
            fDelta = -DirectX::XM_PI / 2.0f + 0.0001f;//カメラ縦方向角度変更
        }
        else
        {
            fDelta += (mousepoint_b.y - mousepoint_a.y) * 0.003f;//カメラ縦方向角度変更
        }

        GetCursorPos(&mousepoint_a);//マウスのスクリーン座標取得
    }
    //カメラ位置決定
    fCameraX = fDistance * cos(fDelta) * cos(fTheta);
    fCameraY = fDistance * sin(fDelta);
    fCameraZ = fDistance * cos(fDelta) * sin(fTheta);

    //------------------------------------------------------------
    // 文字操作
    //------------------------------------------------------------
    //FPS表示用
    WCHAR wcText1[256] = { 0 };
    swprintf(wcText1, 256, L"FPS=%lf", Window::GetFps());
    //カメラ角度表示用
    WCHAR wcText2[256] = { 0 };
    swprintf(wcText2, 256, L"fTheta=%f, fDelta=%f", fTheta, fDelta);
    //カメラ位置表示用
    WCHAR wcText3[256] = { 0 };
    swprintf(wcText3, 256, L"fCameraX=%f, fCameraY=%f, fCameraZ=%f", fCameraX, fCameraY, fCameraZ);

    //------------------------------------------------------------
    // 3D描画
    //------------------------------------------------------------
    D3D11_MAPPED_SUBRESOURCE msr;
    //ビューマトリックスの設定
    DirectX::XMVECTOR vecEye = DirectX::XMVectorSet(fCameraX, fCameraY, fCameraZ, 0.0f);//カメラの位置
    DirectX::XMVECTOR vecFocus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);//カメラの焦点
    DirectX::XMVECTOR vecUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//カメラの上方向
    m_matView = DirectX::XMMatrixLookAtLH(vecEye, vecFocus, vecUp);

    //カメラの更新
    ConstantBuffer cb;
    cb.world = DirectX::XMMatrixTranspose(m_matWorld);
    cb.view = DirectX::XMMatrixTranspose(m_matView);
    cb.projection = DirectX::XMMatrixTranspose(m_matProjection);
    cb.lightpos = DirectX::XMVectorSet(-1, 1, -2, 1);
    cb.eyepos = vecEye;//★---追加---
    m_D3DDeviceContext->Map(m_D3DConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, (void*)(&cb), sizeof(cb));
    m_D3DDeviceContext->Unmap(m_D3DConstantBuffer.Get(), 0);

    mGameMesh1.Draw();
    //DirectX::XMFLOAT3 pos;
    //pos.x = 0; pos.y = 0; pos.z = 0;
    //DirectX::XMFLOAT3 scale;
    //scale.x = 1; scale.y = 1; scale.z = 1;
    //DirectX::XMFLOAT3 degree;
    //degree.x = 0; degree.y = 0; degree.z = 0;
    //mGameMesh1.Draw(cb, pos, scale, degree);

    //pos.x = 1; pos.y = 0; pos.z = 0;
    //mGameMesh2.Draw(cb, pos, scale, degree);
    //------------------------------------------------------------
    // 2D描画
    //------------------------------------------------------------
    m_D2DDeviceContext->BeginDraw();
    m_D2DDeviceContext->DrawText(wcText1, ARRAYSIZE(wcText1) - 1, m_DWriteTextFormat.Get(), D2D1::RectF(0, 0, 800, 20), m_D2DSolidBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);//m_DWriteTextFormatではなくm_DWriteTextFormat.Get()
    m_D2DDeviceContext->DrawText(wcText2, ARRAYSIZE(wcText2) - 1, m_DWriteTextFormat.Get(), D2D1::RectF(0, 20, 800, 40), m_D2DSolidBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);//m_DWriteTextFormatではなくm_DWriteTextFormat.Get()
    m_D2DDeviceContext->DrawText(wcText3, ARRAYSIZE(wcText3) - 1, m_DWriteTextFormat.Get(), D2D1::RectF(0, 40, 800, 60), m_D2DSolidBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);//m_DWriteTextFormatではなくm_DWriteTextFormat.Get()
    m_D2DDeviceContext->EndDraw();

    m_DXGISwapChain1->Present(0, 0);
}

ID3D11DeviceContext* DirectX11::GetDeviceContext3D()
{
    return m_D3DDeviceContext.Get();
}

ID3D11Device* DirectX11::GetDevice3D()
{
    return m_D3DDevice.Get();
}

ID3DBlob* DirectX11::GetVertexShaderBlob()
{
    return m_D3DBlobVS.Get();
}

ID3D11VertexShader* DirectX11::GetVertexShader()
{
    return m_D3DVertexShader.Get();
}

ID3D11PixelShader* DirectX11::GetPixelShader()
{
    return m_D3DPixelShader.Get();
}

ID3D11Buffer** DirectX11::GetConstantBuffer()
{
    return m_D3DConstantBuffer.GetAddressOf();
}
