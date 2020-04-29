#pragma once

#include "Mesh.h"

class CGameObject
{
public: 
	CGameObject() {} 
	~CGameObject();
private: 
	//게임 객체의 위치(월드 좌표계)이다. 
	float m_fxPosition = 0.0f, m_fyPosition = 0.0f, m_fzPosition = 0.0f;
	//게임 객체의 x-축, y-축, z-축 회전 양(축을 기준으로 반시계 방향)이다. 
	float m_fxRotation = 0.0f, m_fyRotation = 0.0f, m_fzRotation = 0.0f; 
	//게임 객체의 x-축, y-축, z-축 회전 양이다. 
	float m_fxRotationSpeed = 0.0f, m_fyRotationSpeed = 0.0f, m_fzRotationSpeed = 0.0f;
	//게임 객체의 모양(메쉬, 모델)이다. 
	CMesh *m_pMesh = nullptr;        
	//게임 객체의 색상(선분의 색상)이다. 
	DWORD m_dwColor = RGB(255, 0, 0);
public: 
	void SetMesh(CMesh* pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z) { m_fxPosition = x; m_fyPosition = y; m_fzPosition = z; } 
	void SetRotation(float x, float y, float z) { m_fxRotation = x; m_fyRotation = y; m_fzRotation = z; } 
	void SetRotationSpeed(float x, float y, float z) { m_fxRotationSpeed = x; m_fyRotationSpeed = y; m_fzRotationSpeed = z; }
	//게임 객체를 x-축, y-축, z-축으로 이동한다.
	void Move(float x, float y, float z) { m_fxPosition += x; m_fyPosition += y; m_fyPosition += z; } 
	//게임 객체를 x-축, y-축, z-축을 기준으로 회전한다. 
	void Rotate(float x, float y, float z) { m_fxRotation += x; m_fyRotation += y; m_fzRotation += z; }
public: 
	//메쉬의 정점 하나를 게임 객체의 위치와 방향을 사용하여 월드 좌표 변환을 한다. 
	CPoint3D WorldTransform(CPoint3D& f3Model);
	//게임 객체를 애니메이션 한다. 
	virtual void Animate(float fElapsedTime); 
	//게임 객체를 렌더링한다. 
	virtual void Render(HDC hDCFrameBuffer); 
};