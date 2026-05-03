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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //5番目のパラメータ：先頭からのバイト数4バイト(FLAOT)×3(RGB)=12
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color });
    mVertices.push_back(SimpleVertex{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f) ,DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) ,_color });

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
    mIndices.push_back(2);
    mIndices.push_back(1);
    mIndices.push_back(2);
    mIndices.push_back(1);
    mIndices.push_back(3);

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * mIndices.size();
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    sub.pSysMem = &mIndices.front();
    hr = device->CreateBuffer(&bd, &sub, &mD3DIndexBuffer);
    if (FAILED(hr))
        return;

    //コンスタントバッファの作成
    //D3D11_BUFFER_DESC bd = {};
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.ByteWidth = sizeof(SimpleVertex) * m_iVertexNum;
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

    //球体の描画
    deviceContext3D->DrawIndexed(6, 0, 0);
}