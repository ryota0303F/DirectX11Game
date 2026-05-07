#include"GameMesh.h"
#include"DirectX.h"
#include "Camera.h"

void GameMesh::Load(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _color)
{
    pos = _pos;
    HRESULT hr = S_OK;
    ID3D11Device* device = DX11.GetDevice3D();
    ID3D11DeviceContext* deviceContext3D = DX11.GetDeviceContext3D();
    ID3DBlob* blobVS = DX11.GetVertexShaderBlob();

    //インプットレイアウトの定義
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT uiElements = ARRAYSIZE(layout);

    //インプットレイアウトの作成
    hr = device->CreateInputLayout(layout, uiElements, blobVS->GetBufferPointer(),
        blobVS->GetBufferSize(), &mD3DInputLayout);
    if (FAILED(hr))
        return;


    //頂点バッファ
    std::vector<SimpleVertex> mVertices;
    //インデックスバッファ
    std::vector<WORD> mIndices;

    //バーテックスバッファの作成
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color ,DirectX::XMFLOAT2(0.0f, 0.0f) });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color ,DirectX::XMFLOAT2(1.0f, 0.0f) });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color ,DirectX::XMFLOAT2(0.0f, 1.0f) });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color ,DirectX::XMFLOAT2(1.0f, 1.0f) });

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(SimpleVertex) * mVertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = &mVertices.front();
    hr = device->CreateBuffer(&bd, &sub, &mD3DVertexBuffer);
    if (FAILED(hr))
        return;

    //インデックスバッファの作成
    mIndices.push_back(0);
    mIndices.push_back(1);
    mIndices.push_back(3);
    mIndices.push_back(0);
    mIndices.push_back(3);
    mIndices.push_back(2);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * mIndices.size();
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    sub.pSysMem = &mIndices.front();
    hr = device->CreateBuffer(&bd, &sub, &mD3DIndexBuffer);
    if (FAILED(hr))
        return;

    //コンスタントバッファの作成
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&bd, nullptr, &m_D3DConstantBuffer);
    if (FAILED(hr))
        return;

    //ラスタライザの作成
    D3D11_RASTERIZER_DESC ras = {};
    //D3D11_FILL_WIREFRAME（ワイヤーフレーム） D3D11_FILL_SOLID（ソリッド）
    ras.FillMode = D3D11_FILL_SOLID;
    //D3D11_CULL_NONE（カリングなし：裏表描画） D3D11_CULL_FRONT（表面カリング：裏面描画） D3D11_CULL_BACK（裏面カリング：表面描画）
    ras.CullMode = D3D11_CULL_BACK;
    ras.FrontCounterClockwise = TRUE;
    hr = device->CreateRasterizerState(&ras, &mD3DRasterizerState);
    if (FAILED(hr))
        return;

    //テクスチャの読み込み
    Microsoft::WRL::ComPtr<IWICImagingFactory> WICImagingFactory;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)(&WICImagingFactory));
    if (FAILED(hr))
        return;

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> WICBitmapDecoder;
    //関数CreateDecoderFromFilename()
    //第1引数：ファイル名
    hr = WICImagingFactory->CreateDecoderFromFilename(L"Resource/image.png", nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &WICBitmapDecoder);
    if (FAILED(hr))
        return;

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> WICBitmapFrameDecode;
    hr = WICBitmapDecoder->GetFrame(0, &WICBitmapFrameDecode);
    if (FAILED(hr))
        return;

    Microsoft::WRL::ComPtr<IWICFormatConverter> WICFormatConverter;
    hr = WICImagingFactory->CreateFormatConverter(&WICFormatConverter);
    if (FAILED(hr))
        return;

    hr = WICFormatConverter->Initialize(WICBitmapFrameDecode.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 1.0f, WICBitmapPaletteTypeMedianCut);
    if (FAILED(hr))
        return;

    //テクスチャのサイズを取得
    UINT uiImageWidth;
    UINT uiImageHeight;
    hr = WICFormatConverter->GetSize(&uiImageWidth, &uiImageHeight);
    if (FAILED(hr))
        return;

    //テクスチャの作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> D3DTexture;
    D3D11_TEXTURE2D_DESC td;
    td.Width = uiImageWidth;
    td.Height = uiImageHeight;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DYNAMIC;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    td.MiscFlags = 0;
    hr = device->CreateTexture2D(&td, nullptr, &D3DTexture);
    if (FAILED(hr))
        return;

    deviceContext3D->Map(D3DTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    WICFormatConverter->CopyPixels(nullptr, uiImageWidth * 4, uiImageWidth * uiImageHeight * 4, (BYTE*)msr.pData);
    deviceContext3D->Unmap(D3DTexture.Get(), 0);

    //シェーダリソースビューの作成
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> D3DShaderResourceView;
    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
    srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(D3DTexture.Get(), &srv, &D3DShaderResourceView);
    if (FAILED(hr))
        return;

    //サンプラステートの設定
    //Microsoft::WRL::ComPtr<ID3D11SamplerState> D3DSamplerState;
    D3D11_SAMPLER_DESC sd = {};
    //補間方法　D3D11_FILTER_MIN_MAG_MIP_POINT（ポイントサンプリング）、D3D11_FILTER_MIN_MAG_MIP_LINEAR（線形補間）、D3D11_FILTER_ANISOTROPIC（異方性補間）等
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //0～1の範囲外テクスチャ　D3D11_TEXTURE_ADDRESS_WRAP（並べる）、D3D11_TEXTURE_ADDRESS_MIRROR（反転）等
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = device->CreateSamplerState(&sd, &D3DSamplerState);
    if (FAILED(hr))
        return;
}

void GameMesh::Draw()
{
    ID3D11DeviceContext* deviceContext3D = DX11.GetDeviceContext3D();
    ID3D11VertexShader* vertexShader = DX11.GetVertexShader();
    ID3D11PixelShader* pixelShader = DX11.GetPixelShader();

    cb.world = DirectX::XMMatrixTranspose(
        DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z)
    );
    //cb.world = DirectX::XMMatrixTranspose(m_matWorld);
    cb.view = DirectX::XMMatrixTranspose(GameCamera.GetViewMatrix());
    cb.projection = DirectX::XMMatrixTranspose(GameCamera.GetProjectionMatrix());
    cb.lightpos = DirectX::XMVectorSet(-1, 1, -2, 1);
    cb.eyepos = GameCamera.GetEye();

    deviceContext3D->Map(m_D3DConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, (void*)(&cb), sizeof(cb));
    deviceContext3D->Unmap(m_D3DConstantBuffer.Get(), 0);

    //ラスタライザの設定
    deviceContext3D->RSSetState(mD3DRasterizerState.Get());
    //インプットレイアウトの設定
    deviceContext3D->IASetInputLayout(mD3DInputLayout.Get());

    //バーテックスバッファの設定
    UINT uiStride = sizeof(SimpleVertex);
    UINT uiOffset = 0;
    deviceContext3D->IASetVertexBuffers(0, 1, mD3DVertexBuffer.GetAddressOf(), &uiStride, &uiOffset);

    //インデックスバッファの設定
    deviceContext3D->IASetIndexBuffer(mD3DIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    //シェーダのセット
    deviceContext3D->VSSetShader(vertexShader, nullptr, 0);
    deviceContext3D->VSSetConstantBuffers(0, 1, m_D3DConstantBuffer.GetAddressOf());
    deviceContext3D->PSSetShader(pixelShader, nullptr, 0);
    deviceContext3D->PSSetConstantBuffers(0, 1, m_D3DConstantBuffer.GetAddressOf());
    deviceContext3D->PSSetSamplers(0, 1, D3DSamplerState.GetAddressOf());//★---追加---
    deviceContext3D->PSSetShaderResources(0, 1, D3DShaderResourceView.GetAddressOf());//★---追加---

    //球体の描画
    deviceContext3D->DrawIndexed(6, 0, 0);
}