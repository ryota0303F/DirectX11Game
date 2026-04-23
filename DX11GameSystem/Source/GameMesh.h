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
private:
	//バーテックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DVertexBuffer;
	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DIndexBuffer;
	//インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mD3DInputLayout;
	//ラスタライザの作成
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mD3DRasterizerState;
};