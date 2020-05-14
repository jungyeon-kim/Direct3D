#pragma once

#include "Mesh.h"

class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();
public:
	bool m_bActive = true;
	//���� ��ü�� ���(�޽�, ��)�̴�. 
	CMesh* m_pMesh = NULL;
	//���� ��ü�� ���� ��ȯ ����̴�. 
	XMFLOAT4X4 m_xmf4x4World = Matrix4x4::Identity();
	//���� ��ü�� ����(������ ����)�̴�. 
	DWORD m_dwColor = RGB(255, 0, 0);
	//���� ��ü�� �̵� ������ ��Ÿ���� �����̴�. 
	XMFLOAT3 m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	float m_fMovingSpeed = 0.0f;
	float m_fMovingRange = 0.0f;
	//���� ��ü�� ȸ������ ��Ÿ���� �����̴�. 
	XMFLOAT3 m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	float m_fRotationSpeed = 0.0f;
public:
	void SetMesh(CMesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetActive(bool bActive) { m_bActive = bActive; } 
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z); 
	void SetPosition(XMFLOAT3& xmf3Position);
	void SetMovingDirection(const XMFLOAT3& xmf3MovingDirection);
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; } 
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }
	void SetRotationAxis(const XMFLOAT3& xmf3RotationAxis);
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }
	void Move(XMFLOAT3& vDirection, float fSpeed);
	void Rotate(float fPitch, float fYaw, float fRoll); 
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);
	virtual void OnUpdateTransform() { } 
	virtual void Animate(float fElapsedTime); 
	virtual void Render(HDC hDCFrameBuffer, class CCamera* pCamera);
};