#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <map>
#include <string>
#include <wrl/client.h>

struct SimpleVertex;
struct ConstantBuffer;

class GameMesh
{
public:
	void Load();
	void Draw();
	void Draw(ConstantBuffer cb, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 degree);
private:
	//バーテックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DVertexBuffer;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DIndexBuffer;
	//インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mD3DInputLayout;
	//ラスタライザの作成
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mD3DRasterizerState;
	const int m_iUMax = 48;
	const int m_iVMax = 24;
	int m_iVertexNum = m_iUMax * (m_iVMax + 1);
	int m_iIndexNum = 2 * m_iVMax * (m_iUMax + 1);
};