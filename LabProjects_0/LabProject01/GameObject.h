#pragma once

#include "Mesh.h"

class CGameObject
{
public:
	CGameObject() { }
	~CGameObject();

private:
	XMFLOAT3 Position{};
	XMFLOAT3 Rotation{};
	XMFLOAT3 RotationSpeed{};

    CMesh					*m_pMesh = NULL;        
	DWORD					m_dwColor = RGB(255, 0, 0);

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }

	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	
	void SetPosition(float x, float y, float z) { Position.x = x; Position.y = y; Position.z = z; }
	void SetRotation(float x, float y, float z) { Rotation.x = x; Rotation.y = y; Rotation.z = z; }
	void SetRotationSpeed(float x, float y, float z) { RotationSpeed = Vector3::Add(RotationSpeed, XMFLOAT3(x, y, z)); }

	void Move(float x, float y, float z) { Position = Vector3::Add(Position, XMFLOAT3(x, y, z)); }
	void Rotate(float x, float y, float z) { Rotation = Vector3::Add(Rotation, XMFLOAT3(x, y, z)); }

public:
	CPoint3D WorldTransform(CPoint3D& f3Model);

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer);
};

