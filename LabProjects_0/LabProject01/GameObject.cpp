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
		const XMFLOAT3X3 RotMatrix{ Matrix3x3::Multiply(
			XMFLOAT3X3(0.f, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(1.f, 0.f, 0.f, 0.f, XMScalarCos(fPitch), XMScalarSin(fPitch), 0.f, -XMScalarSin(fPitch), XMScalarCos(fPitch))
			) };
		const XMFLOAT3& RotVector{ Vector3::Add(
			XMFLOAT3(0.f, RotMatrix._22, RotMatrix._23),
			XMFLOAT3(0.f, RotMatrix._32, RotMatrix._33)
			) };
		f3Rotated.y = RotVector.y;
		f3Rotated.z = RotVector.z;
		f3World.y = f3Rotated.y;
		f3World.z = f3Rotated.z;
	}
	if (fYaw != 0.0f)
	{
		const XMFLOAT3X3 RotMatrix{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(XMScalarCos(fYaw), 0.f, XMScalarSin(fYaw), 0.f, 1.f, 0.f, -XMScalarSin(fYaw), 0.f, XMScalarCos(fYaw))
			) };
		const XMFLOAT3& RotVector{ Vector3::Add(
			XMFLOAT3(RotMatrix._11, 0.f, RotMatrix._13),
			XMFLOAT3(RotMatrix._31, 0.f, RotMatrix._33)
			) };
		f3Rotated.x = RotVector.x;
		f3Rotated.z = RotVector.z;
		f3World.x = f3Rotated.x;
		f3World.z = f3Rotated.z;
	}
	if (fRoll != 0.0f)
	{
		const XMFLOAT3X3 RotMatrix{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, 0.f),
			XMFLOAT3X3(XMScalarCos(fRoll), XMScalarSin(fRoll), 0.f, -XMScalarSin(fRoll), XMScalarCos(fRoll), 0.f, 0.f, 0.f, 1.f)
			) };
		const XMFLOAT3& RotVector{ Vector3::Add(
			XMFLOAT3(RotMatrix._11, RotMatrix._12, 0.f),
			XMFLOAT3(RotMatrix._21, RotMatrix._22, 0.f)
			) };
		f3Rotated.x = RotVector.x;
		f3Rotated.y = RotVector.y;
		f3World.x = f3Rotated.x;
		f3World.y = f3Rotated.y;
	}

	const XMFLOAT3& PosVector{ Vector3::Add(XMFLOAT3(f3World.x, f3World.y, f3World.z), Position) };
	f3World.x = PosVector.x;
	f3World.y = PosVector.y;
	f3World.z = PosVector.z;

	return(f3World);
}

void CGameObject::Animate(float fElapsedTime)
{
	XMFLOAT3 RotVector{ Vector3::Multiply(RotationSpeed, XMFLOAT3(fElapsedTime, fElapsedTime, fElapsedTime)) };
	Rotate(RotVector.x, RotVector.y, RotVector.z);
}

void CGameObject::Render(HDC hDCFrameBuffer)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

	if (m_pMesh) m_pMesh->Render(hDCFrameBuffer);

	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
