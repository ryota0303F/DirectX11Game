#include"GameMesh.h"
#include"DirectX.h"

void GameMesh::Init()
{

}

void GameMesh::Draw()
{
	DX11.GetDeviceContext3D()->DrawIndexed(m_iIndexNum, 0, 0);
}
