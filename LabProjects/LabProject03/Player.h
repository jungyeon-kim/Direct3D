#pragma once

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject 
{
public: 
	CPlayer() { }
	virtual ~CPlayer() { if (m_pCamera) delete m_pCamera; }
public: 
	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�. 
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f); 
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f); 
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//3��Ī ī�޶��� ������(Offset) �����̴�. 
	XMFLOAT3 m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f); 
	//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�. 
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����. 
	float m_fFriction = 125.0f;
	//�÷��̾ ���� x-��(Right), y-��(Up), z-��(Look)���� �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����. 
	float m_fPitch = 0.0f; float m_fYaw = 0.0f; float m_fRoll = 0.0f;
	//�÷��̾ ���� ������ ī�޶��̴�. 
	CCamera* m_pCamera = NULL;
public: 
	void SetPosition(float x, float y, float z); 
	void SetRotation(float x, float y, float z);
	//�÷��̾ � ������ ���ϵ��� �ϴ� �Լ��̴�. 
	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	//�÷��̾ �̵��ϴ� �Լ��̴�. 
	void Move(DWORD dwDirection, float fDistance); 
	void Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity); 
	void Move(float x, float y, float z);
	//�÷��̾ ȸ���ϴ� �Լ��̴�. 
	void Rotate(float fPitch, float fYaw, float fRoll);
	void SetCameraOffset(const XMFLOAT3& xmf3CameraOffset);
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�. 
	void Update(float fTimeElapsed = 0.016f);
	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�. 
	virtual void OnUpdateTransform(); 
	virtual void Animate(float fElapsedTime);
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }

private:
	BoundingBox playerBox{};
	std::vector<CGameObject*> bullets{};
	std::vector<BoundingBox> bulletBoxes{};
public:
	void Shot();
	void ReleaseBullet(int index) { bullets.erase(std::remove(bullets.begin(), bullets.end(), bullets[index]), bullets.end()); }
	size_t GetNumOfBullet() const { return bullets.size(); }
	CGameObject* GetBullet(int index) { return bullets[index]; }
	BoundingBox& GetPlayerBox() { return playerBox; }
	BoundingBox& GetBulletBox(int index) { return bulletBoxes[index]; }
	void SetPlayerBox();
};

class CAirplanePlayer : public CPlayer 
{
public: 
	CAirplanePlayer(); 
	virtual ~CAirplanePlayer();
	virtual void OnUpdateTransform();
};
