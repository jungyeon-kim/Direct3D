#pragma once

class CGameObject;
class CCamera;

class CScene
{
public: 
	CScene() {} 
	virtual ~CScene() {}
private: 
	//게임 객체들의 개수와 게임 객체들의 리스트(배열)이다. 
	int m_nObjects = 0; 
	CGameObject **m_ppObjects = nullptr;
public: 
	//게임 객체들을 생성하고 소멸한다. 
	virtual void BuildObjects(); 
	virtual void ReleaseObjects();
	//게임 객체들을 애니메이션한다. 
	virtual void Animate(float fElapsedTime); 
	//게임 객체들을 렌더링한다. 
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera); 
};

