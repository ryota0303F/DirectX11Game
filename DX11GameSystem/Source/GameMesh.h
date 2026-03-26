#pragma once

class GameMesh
{
public:
	void Init();
	void Draw();

private:
	const int m_iUMax = 48;
	const int m_iVMax = 24;
	int m_iVertexNum = m_iUMax * (m_iVMax + 1);
	int m_iIndexNum = 2 * m_iVMax * (m_iUMax + 1);
};