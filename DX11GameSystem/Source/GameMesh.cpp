#include"GameMesh.h"
#include"DirectX.h"

void GameMesh::Load()
{
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
    };
    UINT uiElements = ARRAYSIZE(layout);

    //インプットレイアウトの作成
    hr = device->CreateInputLayout(layout, uiElements, blobVS->GetBufferPointer(),
        blobVS->GetBufferSize(), &mD3DInputLayout);
    if (FAILED(hr))
        return;


    //頂点バッファ
    std::vector<SimpleVertex> mVertices(m_iVertexNum);
    //インデックスバッファ
    std::vector<WORD> mIndices(m_iIndexNum);

    //バーテックスバッファの作成
    for (int v = 0; v <= m_iVMax; v++)
    {
        for (int u = 0; u < m_iUMax; u++)
        {
            double dTheta = DirectX::XMConvertToRadians(180.0f * v / m_iVMax);
            double dPhi = DirectX::XMConvertToRadians(360.0f * u / m_iUMax);
            FLOAT fX = static_cast<FLOAT>(sin(dTheta) * cos(dPhi));
            FLOAT fY = static_cast<FLOAT>(cos(dTheta));
            FLOAT fZ = static_cast<FLOAT>(sin(dTheta) * sin(dPhi));
            mVertices[m_iUMax * v + u].Pos = DirectX::XMFLOAT3(fX, fY, fZ);
            mVertices[m_iUMax * v + u].Normal = DirectX::XMFLOAT3(fX, fY, fZ);
        }
    }
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(SimpleVertex) * m_iVertexNum;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sub = {};
    sub.pSysMem = &mVertices.front();
    hr = device->CreateBuffer(&bd, &sub, &mD3DVertexBuffer);
    if (FAILED(hr))
        return;

    //インデックスバッファの作成
    int i = 0;
    for (int v = 0; v < m_iVMax; v++)
    {
        for (int u = 0; u <= m_iUMax; u++)
        {
            if (u == m_iUMax)
            {
                mIndices[i] = v * m_iUMax;
                i++;
                mIndices[i] = (v + 1) * m_iUMax;
                i++;
            }
            else
            {
                mIndices[i] = (v * m_iUMax) + u;
                i++;
                mIndices[i] = mIndices[i - 1] + m_iUMax;
                i++;
            }
        }
    }
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * m_iIndexNum;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    sub.pSysMem = &mIndices.front();
    hr = device->CreateBuffer(&bd, &sub, &mD3DIndexBuffer);
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
    ID3D11Buffer** constantBuffer = DX11.GetConstantBuffer();

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
    deviceContext3D->VSSetConstantBuffers(0, 1, constantBuffer);
    deviceContext3D->PSSetShader(pixelShader, nullptr, 0);
    deviceContext3D->PSSetConstantBuffers(0, 1, constantBuffer);

    //球体の描画
    deviceContext3D->DrawIndexed(m_iIndexNum, 0, 0);
}

void GameMesh::Draw(ConstantBuffer cb,DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 degree)
{
    //ID3D11DeviceContext* deviceContext3D = DX11.GetDeviceContext3D();
    //ID3D11VertexShader* vertexShader = DX11.GetVertexShader();
    //ID3D11PixelShader* pixelShader = DX11.GetPixelShader();
    //ID3D11Buffer** constantBuffer = DX11.GetConstantBuffer();

    //// ワールドマトリクス設定
    //DirectX::XMMATRIX world_matrix;
    //DirectX::XMMATRIX translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
    //DirectX::XMMATRIX rotate_x = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(degree.x));
    //DirectX::XMMATRIX rotate_y = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(degree.y));
    //DirectX::XMMATRIX rotate_z = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(degree.z));
    //DirectX::XMMATRIX scale_mat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    //world_matrix = scale_mat * rotate_x * rotate_y * rotate_z * translate;

    //// ワールドマトリクスをコンスタントバッファに設定
    //cb.world = XMMatrixTranspose(world_matrix);

    //// コンスタントバッファ更新
    //deviceContext3D->UpdateSubresource(*constantBuffer, 0, NULL, &cb, 0, 0);

    ////ラスタライザの設定
    //deviceContext3D->RSSetState(mD3DRasterizerState.Get());
    ////インプットレイアウトの設定
    //deviceContext3D->IASetInputLayout(mD3DInputLayout.Get());

    ////バーテックスバッファの設定
    //UINT uiStride = sizeof(SimpleVertex);
    //UINT uiOffset = 0;
    //deviceContext3D->IASetVertexBuffers(0, 1, mD3DVertexBuffer.GetAddressOf(), &uiStride, &uiOffset);

    ////インデックスバッファの設定
    //deviceContext3D->IASetIndexBuffer(mD3DIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    ////シェーダのセット
    //deviceContext3D->VSSetShader(vertexShader, nullptr, 0);
    //deviceContext3D->VSSetConstantBuffers(0, 1, constantBuffer);
    //deviceContext3D->PSSetShader(pixelShader, nullptr, 0);
    //deviceContext3D->PSSetConstantBuffers(0, 1, constantBuffer);

    ////球体の描画
    //deviceContext3D->DrawIndexed(m_iIndexNum, 0, 0);
}
