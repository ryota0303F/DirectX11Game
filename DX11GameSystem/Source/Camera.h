#pragma once

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#include <d3d11_1.h>
#include <DirectXMath.h>


class Camera
{
public:
	static Camera& Instance();
	~Camera();
	void Update();
	DirectX::XMMATRIX GetMatrix();
	DirectX::XMMATRIX* GetMatrixPtr();
	DirectX::XMVECTOR GetEye();
private:
	Camera();
	DirectX::XMVECTOR vecEye;
	DirectX::XMVECTOR vecFocus;
	DirectX::XMVECTOR vecUp;

	DirectX::XMMATRIX m_matView;
};

#define GameCamera Camera::Instance()