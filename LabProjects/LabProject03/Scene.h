#pragma once

class CPlayer;

class CScene {
public: 
	CScene(CPlayer* pPlayer) { m_pPlayer = pPlayer; } virtual ~CScene() { }
private: 
	static constexpr int nObjects{ 5 };
	class CGameObject** m_ppObjects = NULL;
	CPlayer* m_pPlayer = NULL;
public:
	virtual void BuildObjects(); 
	virtual void ReleaseObjects();
	virtual void Animate(float fElapsedTime); 
	virtual void Render(HDC hDCFrameBuffer, class CCamera* pCamera);
	//윈도우 메시지(키보드, 마우스)를 처리한다. 
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { } 
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { } 

private:
	static constexpr int numOfTile{ 30 };

	BoundingBox* objectBox{};
	std::array<XMFLOAT4, 4> tileBox{};
	std::vector<CGameObject*>* particles{};
	std::array<CGameObject*, numOfTile> mapTiles{};
public:
	bool IsCollided(const BoundingBox& left, const BoundingBox& right);
	void ProcessCollision();

	int CheckPicking(XMFLOAT3& rayOrigin, XMFLOAT3& rayDir);

	CGameObject* GetGameObject(int idx) const { return m_ppObjects[idx]; }
};