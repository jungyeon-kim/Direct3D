#pragma once

#include "GameObject.h" 
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer() { m_pCamera = new CCamera(); }
	virtual ~CPlayer() { if (m_pCamera) delete m_pCamera; }
private:
	//�÷��̾� ��ü�� ���Ե� ī�޶��̴�. 
	CCamera* m_pCamera = nullptr;
public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }
};