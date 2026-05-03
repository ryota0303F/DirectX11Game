#pragma once

#include "GraphicsTypes.h"


class Camera
{
public:
	static Camera& Instance();
	~Camera();
	void Update();
	DirectX::XMMATRIX GetWorldMatrix();
	DirectX::XMMATRIX* GetWorldMatrixPtr();
	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX* GetViewMatrixPtr();
	DirectX::XMMATRIX GetProjectionMatrix();
	DirectX::XMMATRIX* GetProjectionMatrixPtr();
	DirectX::XMVECTOR GetEye();
private:
	Camera();
	DirectX::XMVECTOR vecEye;
	DirectX::XMVECTOR vecFocus;
	DirectX::XMVECTOR vecUp;

	DirectX::XMMATRIX m_matWorld;
	DirectX::XMMATRIX m_matView;
	DirectX::XMMATRIX m_matProjection;
};

#define GameCamera Camera::Instance()