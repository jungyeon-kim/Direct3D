#include "stdafx.h"
#include "GameObject.h"

CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

CPoint3D CGameObject::WorldTransform(CPoint3D& f3Model)
{
	float fPitch = XMConvertToRadians(Rotation.x);
	float fYaw = XMConvertToRadians(Rotation.y);
	float fRoll = XMConvertToRadians(Rotation.z);

	CPoint3D f3World = f3Model;
	CPoint3D f3Rotated = f3Model;

	if (fPitch != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(0.f, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(1.f, 0.f, 0.f, 0.f, cos(fPitch), sin(fPitch), 0.f, -sin(fPitch), cos(fPitch))
			) };
		f3Rotated.y = RotResult._22 + RotResult._32;
		f3Rotated.z = RotResult._23 + RotResult._33;;
		f3World.y = f3Rotated.y;
		f3World.z = f3Rotated.z;
	}
	if (fYaw != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(cos(fYaw), 0.f, sin(fYaw), 0.f, 1.f, 0.f, -sin(fYaw), 0.f, cos(fYaw))
			) };
		f3Rotated.x = RotResult._11 + RotResult._31;
		f3Rotated.z = RotResult._13 + RotResult._33;
		f3World.x = f3Rotated.x;
		f3World.z = f3Rotated.z;
	}
	if (fRoll != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, 0.f),
			XMFLOAT3X3(cos(fRoll), sin(fRoll), 0.f, -sin(fRoll), cos(fRoll), 0.f, 0.f, 0.f, 1.f)
			) };
		f3Rotated.x = RotResult._11 + RotResult._21;
		f3Rotated.y = RotResult._12 + RotResult._22;
		f3World.x = f3Rotated.x;
		f3World.y = f3Rotated.y;
	}

	const XMFLOAT3& PosResult{ Vector3::Add(XMFLOAT3(f3World.x, f3World.y, f3World.z), Position) };
	f3World.x = PosResult.x;
	f3World.y = PosResult.y;
	f3World.z = PosResult.z;

	return(f3World);
}

void CGameObject::Animate(float fElapsedTime)
{
	XMFLOAT3 NewRotation{ Vector3::Multiply(RotationSpeed, XMFLOAT3(fElapsedTime, fElapsedTime, fElapsedTime)) };
	Rotate(NewRotation.x, NewRotation.y, NewRotation.z);
}

void CGameObject::Render(HDC hDCFrameBuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
