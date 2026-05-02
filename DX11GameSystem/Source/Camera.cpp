#include "Camera.h"

Camera& Camera::Instance()
{
    static Camera instance;
    return instance;
}

Camera::Camera()
{

}

Camera::~Camera()
{
}

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

    //------------------------------------------------------------
// キー入力関係
//------------------------------------------------------------
//マウス右クリック
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && bMouseR_drag == false)
    {
        bMouseR_drag = true;//マウス右ドラッグフラグ

        GetCursorPos(&mousepoint_a);//マウスのスクリーン座標取得
    }
    else if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000))
    {
        bMouseR_drag = false;//マウス右ドラッグフラグ
    }
    //マウス右ドラッグ
    if (bMouseR_drag)
    {
        GetCursorPos(&mousepoint_b);//マウスのスクリーン座標取得

        fTheta -= (mousepoint_b.x - mousepoint_a.x) * 0.003f;//カメラ横方向角度変更

        if (fDelta + (mousepoint_b.y - mousepoint_a.y) * 0.003f >= DirectX::XM_PI / 2.0f - 0.0001f)
        {
            fDelta = DirectX::XM_PI / 2.0f - 0.0001f;//カメラ縦方向角度変更
        }
        else if (fDelta + (mousepoint_b.y - mousepoint_a.y) * 0.003f <= -DirectX::XM_PI / 2.0f + 0.0001f)
        {
            fDelta = -DirectX::XM_PI / 2.0f + 0.0001f;//カメラ縦方向角度変更
        }
        else
        {
            fDelta += (mousepoint_b.y - mousepoint_a.y) * 0.003f;//カメラ縦方向角度変更
        }

        GetCursorPos(&mousepoint_a);//マウスのスクリーン座標取得
    }
    //カメラ位置決定
    fCameraX = fDistance * cos(fDelta) * cos(fTheta);
    fCameraY = fDistance * sin(fDelta);
    fCameraZ = fDistance * cos(fDelta) * sin(fTheta);

    if (GetAsyncKeyState('A') & 0x8000)
    {
        fCameraAtX -= 0.01f;
        fCameraX -= 0.01f;
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        fCameraX += 0.01f;
        fCameraAtX += 0.01f;
    }

    vecEye = DirectX::XMVectorSet(fCameraX, fCameraY, fCameraZ, 0.0f);//カメラの位置
    vecFocus = DirectX::XMVectorSet(fCameraAtX, fCameraAtY, fCameraAtZ, 0.0f);//カメラの焦点
    vecUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);//カメラの上方向
    m_matView = DirectX::XMMatrixLookAtLH(vecEye, vecFocus, vecUp);
}

DirectX::XMMATRIX Camera::GetMatrix()
{
    return m_matView;
}

DirectX::XMMATRIX* Camera::GetMatrixPtr()
{
    return &m_matView;
}

DirectX::XMVECTOR Camera::GetEye()
{
    return vecEye;
}
