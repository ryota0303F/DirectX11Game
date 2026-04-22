#include "Camera.h"

void Camera::Update()
{
    static FLOAT fTheta = 5.0f;//カメラ横方向角度//★---変更---
    static FLOAT fDelta = -0.06f;//カメラ縦方向角度//★---変更---
    static bool bMouseR_drag;//マウス右ドラッグフラグ
    static FLOAT fDistance = 4;//カメラ位置から焦点までの距離
    static POINT mousepoint_a;//マウス位置
    static POINT mousepoint_b;//マウス位置
    static FLOAT fCameraX = fDistance * cos(fDelta) * cos(fTheta);//カメラの位置X座標
    static FLOAT fCameraY = fDistance * sin(fDelta);//カメラの位置Y座標
    static FLOAT fCameraZ = fDistance * cos(fDelta) * sin(fTheta);//カメラの位置Z座標
    static FLOAT fCameraAtX = 0;
    static FLOAT fCameraAtY = 0;
    static FLOAT fCameraAtZ = 0;

    DirectX::XMVECTOR vecEye = DirectX::XMVectorSet(fCameraX, fCameraY, fCameraZ, 0.0f);//カメラの位置
    DirectX::XMVECTOR vecFocus = DirectX::XMVectorSet(fCameraAtX, fCameraAtY, fCameraAtZ, 0.0f);//カメラの焦点
    DirectX::XMVECTOR vecUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//カメラの上方向
    m_matView = DirectX::XMMatrixLookAtLH(vecEye, vecFocus, vecUp);
}