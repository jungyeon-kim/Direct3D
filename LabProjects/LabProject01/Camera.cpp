#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_pViewport = new CViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_pViewport->m_nWidth) / float(m_pViewport->m_nHeight);
}

void CCamera::SetFOVAngle(float fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void CCamera::Move(float x, float y, float z)
{
	Position = Vector3::Add(Position, XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	Rotation = Vector3::Add(Rotation, XMFLOAT3(fPitch, fYaw, fRoll));
}

CPoint3D CCamera::CameraTransform(CPoint3D& f3World)
{
	CPoint3D f3Camera = f3World;

	const XMFLOAT3& PosResult{ Vector3::Subtract(XMFLOAT3(f3Camera.x, f3Camera.y, f3Camera.z), Position) };
	f3Camera.x = PosResult.x;
	f3Camera.y = PosResult.y;
	f3Camera.z = PosResult.z;

	float fPitch = XMConvertToDegrees(-Rotation.x);
	float fYaw = XMConvertToDegrees(-Rotation.y);
	float fRoll = XMConvertToDegrees(-Rotation.z);

	CPoint3D f3Rotated = f3Camera;
	if (fPitch != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(0.f, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(1.f, 0.f, 0.f, 0.f, cos(fPitch), sin(fPitch), 0.f, -sin(fPitch), cos(fPitch))
			) };
		f3Rotated.y = RotResult._22 + RotResult._32;
		f3Rotated.z = RotResult._23 + RotResult._33;;
		f3Camera.y = f3Rotated.y;
		f3Camera.z = f3Rotated.z;
	}
	if (fYaw != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, f3World.z),
			XMFLOAT3X3(cos(fYaw), 0.f, sin(fYaw), 0.f, 1.f, 0.f, -sin(fYaw), 0.f, cos(fYaw))
			) };
		f3Rotated.x = RotResult._11 + RotResult._31;
		f3Rotated.z = RotResult._13 + RotResult._33;
		f3Camera.x = f3Rotated.x;
		f3Camera.z = f3Rotated.z;
	}
	if (fRoll != 0.0f)
	{
		const XMFLOAT3X3 RotResult{ Matrix3x3::Multiply(
			XMFLOAT3X3(f3World.x, 0.f, 0.f, 0.f, f3World.y, 0.f, 0.f, 0.f, 0.f),
			XMFLOAT3X3(cos(fRoll), sin(fRoll), 0.f, -sin(fRoll), cos(fRoll), 0.f, 0.f, 0.f, 1.f)
			) };
		f3Rotated.x = RotResult._11 + RotResult._21;
		f3Rotated.y = RotResult._12 + RotResult._22;
		f3Camera.x = f3Rotated.x;
		f3Camera.y = f3Rotated.y;
	}

	return(f3Camera);
}

CPoint3D CCamera::ProjectionTransform(CPoint3D& f3Camera)
{
	CPoint3D f3Project = f3Camera;
	if (f3Camera.z != 0.0f)
	{
		f3Project.x = float((f3Camera.x * m_fProjectRectDistance) / (m_fAspectRatio * f3Camera.z));
		f3Project.y = float((f3Camera.y * m_fProjectRectDistance) / f3Camera.z);
		f3Project.z = f3Camera.z;
	}

	return(f3Project);
}

CPoint3D CCamera::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen = f3Projection;

	float fHalfWidth = m_pViewport->m_nWidth * 0.5f;
	float fHalfHeight = m_pViewport->m_nHeight * 0.5f;
	f3Screen.x = (f3Projection.x * fHalfWidth) + m_pViewport->m_nLeft + fHalfWidth;
	f3Screen.y = (-f3Projection.y * fHalfHeight) + m_pViewport->m_nTop + fHalfHeight;

	return(f3Screen);
}
