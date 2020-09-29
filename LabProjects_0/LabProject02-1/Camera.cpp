#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"

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
	m_fxPosition += x; 
	m_fyPosition += y; 
	m_fzPosition += z; 
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll) 
{ 
	m_fxRotation += fPitch; 
	m_fyRotation += fYaw; 
	m_fzRotation += fRoll; 
}

CPoint3D CCamera::CameraTransform(CPoint3D& f3World) 
{ 
	//카메라를 월드 좌표계의 원점으로 이동한다. 
	CPoint3D f3Camera = f3World;
	f3Camera.x -= m_fxPosition; 
	f3Camera.y -= m_fyPosition; 
	f3Camera.z -= m_fzPosition;
	float fPitch = DegreeToRadian(-m_fxRotation); 
	float fYaw = DegreeToRadian(-m_fyRotation); 
	float fRoll = DegreeToRadian(-m_fzRotation);

	//카메라를 월드 좌표계의 축과 일치하도록 회전한다. 
	CPoint3D f3Rotated = f3Camera; 
	if (fPitch != 0.0f) 
	{ 
		f3Rotated.y = float(f3Camera.y * cos(fPitch) - f3Camera.z * sin(fPitch)); 
		f3Rotated.z = float(f3Camera.y * sin(fPitch) + f3Camera.z * cos(fPitch)); 
		f3Camera.y = f3Rotated.y; f3Camera.z = f3Rotated.z; 
	} 
	if (fYaw != 0.0f) 
	{ 
		f3Rotated.x = float(f3Camera.x * cos(fYaw) + f3Camera.z * sin(fYaw)); 
		f3Rotated.z = float(-f3Camera.x * sin(fYaw) + f3Camera.z * cos(fYaw)); 
		f3Camera.x = f3Rotated.x; f3Camera.z = f3Rotated.z; 
	} 
	if (fRoll != 0.0f) 
	{ 
		f3Rotated.x = float(f3Camera.x * cos(fRoll) - f3Camera.y * sin(fRoll)); 
		f3Rotated.y = float(f3Camera.x * sin(fRoll) + f3Camera.y * cos(fRoll)); 
		f3Camera.x = f3Rotated.x; f3Camera.y = f3Rotated.y; 
	}

	return(f3Camera);
}

CPoint3D CCamera::ProjectionTransform(CPoint3D& f3Camera)
{
	CPoint3D f3Project = f3Camera;

	if (f3Camera.z != 0.0f)
	{
		//카메라의 시야각이 90°가 아닌 경우 투영 사각형까지의 거리를 곱한다. 
		f3Project.x = float((f3Camera.x * m_fProjectRectDistance) / (m_fAspectRatio * f3Camera.z));
		f3Project.y = float((f3Camera.y * m_fProjectRectDistance) / f3Camera.z);
		//투영 좌표계는 2차원이므로 z-좌표에 카메라 좌표계 z-좌표를 저장한다. 
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
