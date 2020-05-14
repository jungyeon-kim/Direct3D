#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CPlayer::SetPosition(float x, float y, float z) 
{ 
	m_xmf3Position = XMFLOAT3(x, y, z); 
	CGameObject::SetPosition(x, y, z); 
}

void CPlayer::SetRotation(float x, float y, float z)
{
	/*CGameObject::SetRotation(x, y, z);

	if (m_pCamera) m_pCamera->SetRotation(x, y, z);*/
}

void CPlayer::SetCameraOffset(const XMFLOAT3& xmf3CameraOffset) 
{
	m_xmf3CameraOffset = xmf3CameraOffset; 
	XMFLOAT3 xmf3CameraPosition; 
	XMStoreFloat3(&xmf3CameraPosition, XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&m_xmf3CameraOffset))); 
	m_pCamera->SetLookAt(xmf3CameraPosition, m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를 누를 때 변경된다. 
플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만 큼 이동한다.*/ 
void CPlayer::Move(DWORD dwDirection, float fDistance) 
{
	if (dwDirection) 
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_FORWARD) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Look), fDistance))); 
		if (dwDirection & DIR_BACKWARD) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Look), -fDistance))); 
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_RIGHT) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Right), fDistance))); 
		if (dwDirection & DIR_LEFT) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Right), -fDistance))); 
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다. ‘Page Down’을 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_UP) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Up), fDistance))); 
		if (dwDirection & DIR_DOWN) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Up), -fDistance)));
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		Move(xmf3Shift, true); 
	} 
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) 
{
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다. 
	if (bUpdateVelocity) 
	{ 
		//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다. 
		XMStoreFloat3(&m_xmf3Velocity, XMVectorAdd(XMLoadFloat3(&m_xmf3Velocity), XMLoadFloat3(&xmf3Shift))); 
	} 
	else 
	{ 
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		XMStoreFloat3(&m_xmf3Position, XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3Shift))); 
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다. 
		if (m_pCamera) m_pCamera->Move(xmf3Shift); 
	} 
}

void CPlayer::Move(float x, float y, float z) 
{
	Move(XMFLOAT3(x, y, z), false);
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다. 
void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다. 
	m_pCamera->Rotate(fPitch, fYaw, fRoll);

	if (fPitch != 0.0f) 
	{ 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch)); 
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

	/*회전으로 인해 플레이어의 로컬 x-축, y-축, z-축이 서로 직교하지 않을 수 있으므로 z-축(Look 벡 터)을 기준으로 하여 
	서로 직교하고 단위벡터가 되도록 한다.*/ 
	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&m_xmf3Look)); 
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&m_xmf3Up)); 
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook)); 
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));
	XMStoreFloat3(&m_xmf3Right, xmvRight); 
	XMStoreFloat3(&m_xmf3Up, xmvUp);
	XMStoreFloat3(&m_xmf3Look, xmvLook);
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up) 
{
	XMFLOAT4X4 xmf4x4View; 
	XMStoreFloat4x4(&xmf4x4View, XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3LookAt), XMLoadFloat3(&xmf3Up)));
	XMVECTORF32 xmf32vRight = { xmf4x4View._11, xmf4x4View._21, xmf4x4View._31, 0.0f }; 
	XMVECTORF32 xmf32vUp = { xmf4x4View._12, xmf4x4View._22, xmf4x4View._32, 0.0f }; 
	XMVECTORF32 xmf32vLook = { xmf4x4View._13, xmf4x4View._23, xmf4x4View._33, 0.0f };
	XMStoreFloat3(&m_xmf3Right, XMVector3Normalize(xmf32vRight)); 
	XMStoreFloat3(&m_xmf3Up, XMVector3Normalize(xmf32vUp)); 
	XMStoreFloat3(&m_xmf3Look, XMVector3Normalize(xmf32vLook));
}

//이 함수는 매 프레임마다 프레임워크에서 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용 하여 플레이어를 이동한다. 
void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);
	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed); 
	m_pCamera->GenerateViewMatrix();
	/*플레이어의 속도 벡터가 마찰력 때문에 감속이 되어야 한다면 감속 벡터를 생성한다. 
	속도 벡터의 반 대 방향 벡터를 구하고 단위 벡터로 만든다. 마찰 계수를 시간에 비례하도록 하여 마찰력을 구한다. 
	단 위 벡터에 마찰력을 곱하여 감속 벡터를 구한다. 속도 벡터에 감속 벡터를 더하여 속도 벡터를 줄인다. 
	마찰력이 속력보다 크면 속력은 0이 될 것이다.*/
	XMVECTOR xmvVelocity = XMLoadFloat3(&m_xmf3Velocity); 
	XMVECTOR xmvDeceleration = XMVector3Normalize(XMVectorScale(xmvVelocity, -1.0f));
	float fLength = XMVectorGetX(XMVector3Length(xmvVelocity)); 
	float fDeceleration = m_fFriction * fTimeElapsed; 
	if (fDeceleration > fLength) fDeceleration = fLength; 
	XMStoreFloat3(&m_xmf3Velocity, XMVectorAdd(xmvVelocity, XMVectorScale(xmvDeceleration, fDeceleration)));
}

void CPlayer::Animate(float fElapsedTime) 
{ 
	//플레이어의 위치과 방향 벡터로 부터 따라 월드 변환 행렬을 구한다. 
	OnUpdateTransform();
	CGameObject::Animate(fElapsedTime);
}

/*플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 
플레이어의 Right 벡터가 월 드 변환 행렬의 첫 번째 행 벡터, Up 벡터가 두 번째 행 벡터, Look 벡터가 세 번째 행 벡터, 
플레이어 의 위치 벡터가 네 번째 행 벡터가 된다.*/ 
void CPlayer::OnUpdateTransform() 
{ 
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z; 
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z; 
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z; 
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z; 
}

CAirplanePlayer::CAirplanePlayer()
{
}

CAirplanePlayer::~CAirplanePlayer()
{
}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&m_xmf4x4World)));
}
