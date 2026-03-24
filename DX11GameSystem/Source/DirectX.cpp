#include "Main.h"
#include "DirectX.h"

DirectX11::DirectX11()
{
    pd3dDevice = nullptr;
    pd3dDevice1 = nullptr;
    pImmediateContext = nullptr;
    pImmediateContext1 = nullptr;
    pSwapChain = nullptr;
    pSwapChain1 = nullptr;
    pRenderTargetView = nullptr;

    pD2DFactory = nullptr;
    pDWriteFactory = nullptr;
    pTextFormat = nullptr;
    pRT = nullptr;
    pSolidBrush = nullptr;
    pDXGISurface = nullptr;

    pWICImagingFactory = nullptr;
    pWICBitmapDecoder = nullptr;
    pWICBitmapFrameDecode = nullptr;
    pWICFormatConverter = nullptr;
    pD2DBitmap = nullptr;
}

DirectX11::~DirectX11()
{
    if (pD2DBitmap) pD2DBitmap->Release();
    if (pWICFormatConverter) pWICFormatConverter->Release();
    if (pWICBitmapFrameDecode) pWICBitmapFrameDecode->Release();
    if (pWICBitmapDecoder) pWICBitmapDecoder->Release();
    if (pWICImagingFactory) pWICImagingFactory->Release();

    if (pDXGISurface) pDXGISurface->Release();
    if (pSolidBrush) pSolidBrush->Release();
    if (pRT) pRT->Release();
    if (pTextFormat) pTextFormat->Release();
    if (pDWriteFactory) pDWriteFactory->Release();
    if (pD2DFactory) pD2DFactory->Release();

    if (pImmediateContext) pImmediateContext->ClearState();

    if (pRenderTargetView) pRenderTargetView->Release();
    if (pSwapChain1) pSwapChain1->Release();
    if (pSwapChain) pSwapChain->Release();
    if (pImmediateContext1) pImmediateContext1->Release();
    if (pImmediateContext) pImmediateContext->Release();
    if (pd3dDevice1) pd3dDevice1->Release();
    if (pd3dDevice) pd3dDevice->Release();

    CoUninitialize();
}

HRESULT DirectX11::InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(Window::GethWnd(), &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;//DirectX11上でDirect2Dを使用するために必要
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    D3D_DRIVER_TYPE g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &pd3dDevice, &g_featureLevel, &pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &pd3dDevice, &g_featureLevel, &pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        hr = pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(pd3dDevice, Window::GethWnd(), &sd, nullptr, nullptr, &pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = Window::GethWnd();
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(pd3dDevice, &sd, &pSwapChain);
    }

    dxgiFactory->MakeWindowAssociation(Window::GethWnd(), DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pImmediateContext->RSSetViewports(1, &vp);

    // Direct2D,DirectWriteの初期化
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
    if (FAILED(hr))
        return hr;

    hr = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pDXGISurface));
    if (FAILED(hr))
        return hr;

    FLOAT dpiX = (FLOAT)GetDpiForWindow(GetDesktopWindow());
    FLOAT dpiY = (FLOAT)GetDpiForWindow(GetDesktopWindow());

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

    hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(pDXGISurface, &props, &pRT);
    if (FAILED(hr))
        return hr;

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
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
    hr = pDWriteFactory->CreateTextFormat(L"メイリオ", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"", &pTextFormat);
    if (FAILED(hr))
        return hr;

    //関数SetTextAlignment()
    //第1引数：テキストの配置（DWRITE_TEXT_ALIGNMENT_LEADING：前, DWRITE_TEXT_ALIGNMENT_TRAILING：後, DWRITE_TEXT_ALIGNMENT_CENTER：中央）
    hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if (FAILED(hr))
        return hr;

    //関数CreateSolidColorBrush()
    //第1引数：フォント色（D2D1::ColorF(D2D1::ColorF::Black)：黒, D2D1::ColorF(D2D1::ColorF(0.0f, 0.2f, 0.9f, 1.0f))：RGBA指定）
    hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pSolidBrush);
    if (FAILED(hr))
        return hr;

    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pWICImagingFactory);
    if (FAILED(hr))
        return hr;

    //関数CreateDecoderFromFilename()
    //第1引数：ファイル名
    hr = pWICImagingFactory->CreateDecoderFromFilename(L"Resource/test.png", nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pWICBitmapDecoder);
    if (FAILED(hr))
        return hr;

    hr = pWICBitmapDecoder->GetFrame(0, &pWICBitmapFrameDecode);
    if (FAILED(hr))
        return hr;

    hr = pWICImagingFactory->CreateFormatConverter(&pWICFormatConverter);
    if (FAILED(hr))
        return hr;

    hr = pWICFormatConverter->Initialize(pWICBitmapFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 1.0f, WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr))
        return hr;

    hr = pRT->CreateBitmapFromWicBitmap(pWICFormatConverter, nullptr, &pD2DBitmap);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

void DirectX11::Render()
{
    pImmediateContext->ClearRenderTargetView(pRenderTargetView, DirectX::Colors::Aquamarine);

    //------------------------------------------------------------
    // キー入力関係
    //------------------------------------------------------------
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

    //------------------------------------------------------------
    //画像とテキストの描画
    //------------------------------------------------------------
    
    //FPS表示用
    WCHAR wcText1[256] = { 0 };
    swprintf(wcText1, 256, L"FPS=%lf", Window::GetFps());
    //FrameTime表示用
    WCHAR wcText2[256] = { 0 };
    swprintf(wcText2, 256, L"FrameTime=%lf", Window::GetFrameTime());

    float fPosX = fMouseX;
    float fPosY = fMouseY;

    pRT->BeginDraw();
    //関数DrawBitmap()
    //第1引数：レンダリングするビットマップ（pD2DBitmap）
    //第2引数：ビットマップの位置の座標（D2D1::RectF(左, 上, 右, 下)）
    //第3引数：不透明度（0.0f～1.0f）
    //第4引数：ビットマップが拡大縮小または回転される場合に使用する補間モード（D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR：ドット絵風[ギザギザ]
    //                                                                         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR：写真風[なめらか]）
    //第5引数：トリミング（D2D1::RectF(左, 上, 右, 下), nullptr：イメージ全体の場合）
    pRT->DrawBitmap(pD2DBitmap, D2D1::RectF(fPosX, fPosY, fPosX + 128, fPosY + 128), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, nullptr);
    //pRT->DrawBitmap(pD2DBitmap, D2D1::RectF(fPosX, fPosY, fPosX + 128, fPosY + 128), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, nullptr);
    pRT->DrawText(wcText1, ARRAYSIZE(wcText1) - 1, pTextFormat, D2D1::RectF(0, 0, 800, 20), pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
    pRT->DrawText(wcText2, ARRAYSIZE(wcText2) - 1, pTextFormat, D2D1::RectF(0, 20, 800, 40), pSolidBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
    pRT->EndDraw();

    pSwapChain->Present(0, 0);
}