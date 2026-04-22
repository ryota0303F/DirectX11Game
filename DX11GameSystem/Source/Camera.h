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
	void Update();
private:
	DirectX::XMVECTOR vecEy;
	DirectX::XMVECTOR vecFocus;
	DirectX::XMVECTOR vecUp;

	DirectX::XMMATRIX m_matView;
};