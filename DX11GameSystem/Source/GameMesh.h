#pragma once
#include "GraphicsTypes.h"
#include <vector>
#include <map>
#include <string>
#include <wrl/client.h>

struct SimpleVertex;
struct ConstantBuffer;

class GameMesh
{
public:
	void Load(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _color);
	void Draw();
private:
	//バーテックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DVertexBuffer;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DIndexBuffer;
	//インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mD3DInputLayout;
	//ラスタライザの作成
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mD3DRasterizerState;
	//コンスタンスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_D3DConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> D3DSamplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> D3DShaderResourceView;

	ConstantBuffer cb;
	D3D11_MAPPED_SUBRESOURCE msr;

	DirectX::XMFLOAT3 pos;
	
};