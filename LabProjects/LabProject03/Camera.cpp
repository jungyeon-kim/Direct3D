#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CViewport::SetViewport(int nLeft, int nTop, int nWidth, int nHeight) 
{ 
	m_nLeft = nLeft; 
	m_nTop = nTop; 
	m_nWidth = nWidth; 
	m_nHeight = nHeight; 
}

/*ī�޶� ���� �� ȸ���� �ϰ� �Ǹ� ������ �Ǽ� ǥ���� ���� ������ ī�޶��� ���� x-��(Right), y��(Up), z-��(Look)�� 
���� �������� ���� �� �ִ�. ī�޶��� ���� x-��(Right), y-��(Up), z-��(Look) �� ���� �����ϵ��� ������ش�.*/ 
void CCamera::GenerateViewMatrix() 
{ 
	//ī�޶��� z-���� �������� ī�޶��� ��ǥ����� �����ϵ��� �����. 
	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&m_xmf3Look)); 
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&m_xmf3Up)); 
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook)); 
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));
	XMStoreFloat3(&m_xmf3Look, xmvLook); 
	XMStoreFloat3(&m_xmf3Right, xmvRight); 
	XMStoreFloat3(&m_xmf3Up, xmvUp);

	//ī�޶� ��ȯ ����� ī�޶� ���庯ȯ ����� �����(��ġ���)�̴�. 
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x; 
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y; 
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	XMVECTOR xmvPosition = XMLoadFloat3(&m_xmf3Position); 
	m_xmf4x4View._41 = -XMVectorGetX(XMVector3Dot(xmvPosition, xmvRight)); 
	m_xmf4x4View._42 = -XMVectorGetX(XMVector3Dot(xmvPosition, xmvUp)); 
	m_xmf4x4View._43 = -XMVectorGetX(XMVector3Dot(xmvPosition, xmvLook));
	XMStoreFloat4x4(&m_xmf4x4ViewProject, XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4View), XMLoadFloat4x4(&m_xmf4x4Project)));
}

//ī�޶� xmf3LookAt�� �ٶ󺸵��� ī�޶� ��ȯ ����� �����Ѵ�. 
void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up) 
{ 
	m_xmf3Position = xmf3Position; 
	XMStoreFloat4x4(&m_xmf4x4View, XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3LookAt), XMLoadFloat3(&xmf3Up)));

	//ī�޶� ��ȯ ��Ŀ��� ī�޶��� x-��, y-��, z-���� ���Ѵ�. 
	XMVECTORF32 xmf32vRight = { m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31, 0.0f }; 
	XMVECTORF32 xmf32vUp = { m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32, 0.0f }; 
	XMVECTORF32 xmf32vLook = { m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33, 0.0f };
	XMStoreFloat3(&m_xmf3Right, XMVector3Normalize(xmf32vRight)); 
	XMStoreFloat3(&m_xmf3Up, XMVector3Normalize(xmf32vUp)); 
	XMStoreFloat3(&m_xmf3Look, XMVector3Normalize(xmf32vLook));
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight) 
{ 
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight); 
	m_fAspectRatio = float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight); 
}

void CCamera::SetFOVAngle(float fFOVAngle) 
{ 
	m_fFOVAngle = fFOVAngle; 
	m_fProjectRectDistance = float(1.0f / tan(XMConvertToRadians(fFOVAngle * 0.5f))); 
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up) 
{ 
	//���� ī�޶��� ��ġ���� �÷��̾ �ٶ󺸱� ���� ī�޶� ��ȯ ����� �����Ѵ�. 
	SetLookAt(m_xmf3Position, xmf3LookAt, xmf3Up); 
}

//���� ���� ��ȯ ����� �����Ѵ�. 
void CCamera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{ 
	float fAspectRatio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight)); 
	XMStoreFloat4x4(&m_xmf4x4Project, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, 
		fNearPlaneDistance, fFarPlaneDistance)); 
}

//ī�޶� �̵��Ѵ�. 
void CCamera::Move(const XMFLOAT3& xmf3Shift) 
{ 
	XMStoreFloat3(&m_xmf3Position, XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3Shift))); 
}

void CCamera::Move(float x, float y, float z) 
{ 
	Move(XMFLOAT3(x, y, z));
}

//ī�޶��� ���� x-��(Right), y-��(Up), z-��(Look)�� �������� ȸ���ϴ� �Լ��̴�. 
void CCamera::Rotate(float fPitch, float fYaw, float fRoll) 
{ 
	if (fPitch != 0.0f) 
	{ 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch)); 
		//ī�޶��� ���� x-��(Right)�� �߽����� ȸ���ϴ� ��ķ� y-��(Up), z-��(Look)�� ȸ���Ѵ�. 
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate)); 
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate)); 
	} 
	if (fYaw != 0.0f) 
	{ 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw)); 
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate)); 
	} 
	if (fRoll != 0.0f) 
	{ 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll)); 
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate)); 
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate)); 
	} 
}

void CCamera::Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, float fTimeElapsed) 
{ 
	//�÷��̾��� ���� x-��, y-��, z-�� ���ͷκ��� ȸ�� ���(�÷��̾�� ���� ������ ��Ÿ���� ���)�� �����Ѵ�. 
	XMMATRIX xmmtx4Rotate; 
	xmmtx4Rotate.r[0] = XMVectorSet(pPlayer->m_xmf3Right.x, pPlayer->m_xmf3Right.y, pPlayer->m_xmf3Right.z, 0.0f); 
	xmmtx4Rotate.r[1] = XMVectorSet(pPlayer->m_xmf3Up.x, pPlayer->m_xmf3Up.y, pPlayer->m_xmf3Up.z, 0.0f); 
	xmmtx4Rotate.r[2] = XMVectorSet(pPlayer->m_xmf3Look.x, pPlayer->m_xmf3Look.y, pPlayer->m_xmf3Look.z, 0.0f); 
	xmmtx4Rotate.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR xmvPosition = XMLoadFloat3(&m_xmf3Position); 

	//ī�޶� ������ ���͸� ȸ�� ��ķ� ��ȯ(ȸ��)�Ѵ�. 
	XMVECTOR xmvOffset = XMVector3TransformCoord(XMLoadFloat3(&pPlayer->m_xmf3CameraOffset), xmmtx4Rotate);
	//ȸ���� ī�޶��� ��ġ�� �÷��̾��� ��ġ�� ȸ���� ī�޶� ������ ���͸� ���� ���̴�. 
	XMVECTOR xmvNewPosition = XMVectorAdd(XMLoadFloat3(&pPlayer->m_xmf3Position), xmvOffset); 
	//������ ī�޶��� ��ġ���� ȸ���� ī�޶��� ��ġ������ ����� �Ÿ��� ��Ÿ���� �����̴�. 
	XMVECTOR xmvDirection = XMVectorSubtract(xmvNewPosition, xmvPosition); 
	float fLength = XMVectorGetX(XMVector3Length(xmvDirection)); 
	xmvDirection = XMVector3Normalize(xmvDirection);
	/*ī�޶��� ����(Lag)�� �÷��̾ ȸ���ϴ��� ī�޶� ���ÿ� ���� ȸ������ �ʰ� �ణ�� ������ �ΰ� ȸ���ϴ� 
	ȿ���� �����ϱ� ���� ���̴�. ���� �׸����� �÷��̾ ȸ���� �� ī�޶�� c1���� c2 �� ������ �ΰ� õõ��(�����) 
	�̵��ϸ鼭 �÷��̾ �ٶ󺸵��� �Ѵ�. 4.0�� 40.0���� �ٲپ� ȸ�� �� �غ��� ���̸� ���� �� ���� ���̴�.*/
	float fTimeLagScale = fTimeElapsed * 4.0f; 
	float fDistance = fLength * fTimeLagScale; 
	if (fDistance > fLength) fDistance = fLength; 
	if (fLength < 0.01f) fDistance = fLength; if (fDistance > 0) 
	{ 
		//ī�޶� �������� �ʰ� �̵��� �Ѵ�(ȸ���� ������ ���� ��� ȸ�� �̵��� ���� �̵��� ���� ����). 
		XMStoreFloat3(&m_xmf3Position, XMVectorAdd(xmvPosition, XMVectorScale(xmvDirection, fDistance))); 
		//ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�. 
		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up); 
	} 
}