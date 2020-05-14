#pragma once

class CPlayer;

class CScene {
public: 
	CScene(CPlayer* pPlayer) { m_pPlayer = pPlayer; } virtual ~CScene() { }
private: 
	int m_nObjects = 0; 
	class CGameObject** m_ppObjects = NULL;
	CPlayer* m_pPlayer = NULL;
public:
	virtual void BuildObjects(); 
	virtual void ReleaseObjects();
	virtual void Animate(float fElapsedTime); 
	virtual void Render(HDC hDCFrameBuffer, class CCamera* pCamera);
	//������ �޽���(Ű����, ���콺)�� ó���Ѵ�. 
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { } 
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { } 
};