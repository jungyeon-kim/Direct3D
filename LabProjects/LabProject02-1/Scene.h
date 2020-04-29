#pragma once

class CGameObject;
class CCamera;

class CScene
{
public: 
	CScene() {} 
	virtual ~CScene() {}
private: 
	//���� ��ü���� ������ ���� ��ü���� ����Ʈ(�迭)�̴�. 
	int m_nObjects = 0; 
	CGameObject **m_ppObjects = nullptr;
public: 
	//���� ��ü���� �����ϰ� �Ҹ��Ѵ�. 
	virtual void BuildObjects(); 
	virtual void ReleaseObjects();
	//���� ��ü���� �ִϸ��̼��Ѵ�. 
	virtual void Animate(float fElapsedTime); 
	//���� ��ü���� �������Ѵ�. 
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera); 
};

