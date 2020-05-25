//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include "Player.h"
#include "Scene.h"

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) 
{
	::srand(timeGetTime());

	m_hInstance = hInstance; 
	m_hWnd = hMainWnd;

	BuildFrameBuffer();
	BuildObjects();

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

void CGameFramework::OnDestroy() 
{
	ReleaseObjects();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);
}

void CGameFramework::BuildFrameBuffer()
{
	::GetClientRect(m_hWnd, &m_rcClient);

	HDC hDC = ::GetDC(m_hWnd);

    m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top);
    ::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
    ::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{    
    HDC hDC = ::GetDC(m_hWnd);
    ::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::BuildObjects() 
{
	CCamera* pCamera = new CCamera(); 
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT); 
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 200.0f, 60.0f); 
	pCamera->SetFOVAngle(60.0f);

	//����� �޽��� �����ϰ� �÷��̾� ��ü�� �����Ѵ�. 
	CAirplaneMesh* pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer = new CAirplanePlayer(); 
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pAirplaneMesh); 
	m_pPlayer->SetColor(RGB(0, 0, 255)); 
	m_pPlayer->SetCamera(pCamera); 
	m_pPlayer->SetPlayerBox();

	//ī�޶�� �÷��̾� ��ü ���� ������ �÷��̾ �ٶ󺻴�. 
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
	m_pScene = new CScene(m_pPlayer); 
	m_pScene->BuildObjects();
}

void CGameFramework::ReleaseObjects() 
{ 
	//�� ��ü�� ���� ��ü���� �Ҹ��ϰ�, �� ��ü�� �÷��̾� ��ü�� �Ҹ��Ѵ�. 
	if (m_pScene) m_pScene->ReleaseObjects(); 
	if (m_pScene) delete m_pScene; 
	if (m_pPlayer) delete m_pPlayer; 
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID) 
	{ 
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�. 
	case WM_RBUTTONDOWN: 
	case WM_LBUTTONDOWN: 
		::SetCapture(hWnd); 
		::GetCursorPos(&m_ptOldCursorPos); 
		break; 
		//���콺 ĸ�ĸ� �����Ѵ�. 
	case WM_LBUTTONUP: 
	case WM_RBUTTONUP: 
	::ReleaseCapture(); 
	break; 
	case WM_MOUSEMOVE: 
	break; 
	default: 
		if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam); 
		break; 
	} 
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: 
		::PostQuitMessage(0); 
		break; 
		case VK_RETURN: 
		break; 
		case VK_CONTROL: 
			m_pPlayer->Shot();
		break; 
		default: 
			if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam); 
			break;
		} 
	break; 
	default: 
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	/*Ű������ ���� ������ ��ȯ�Ѵ�.
	ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(�� �� x-��), ��/��(���� z-��)�� �̵��Ѵ�.
	��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(�� �� y-��)�� �̵��Ѵ�.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		//Ű �Է��� ������ �÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�). 
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.15f);
	}
	if (::GetCapture() == m_hWnd)
	{
		/*���콺�� ĸ��������(���콺 Ŭ���� �Ͼ����) ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�.
		���콺 ���� �Ǵ� ������ ��ư�� ������ ���� �޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���.
		�׷��Ƿ� ���콺�� ĸ�ĵ� �����찡 ���� �������̸� ���콺 ��ư�� ���� ���� ���� Ŭ���̾�Ʈ �������� ������ ���¸�
		�ǹ��Ѵ�. ���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� �÷��̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/
		//���콺 Ŀ���� ȭ�鿡�� ���ش�(������ �ʰ� �Ѵ�). 
		::SetCursor(NULL);
		POINT ptCursorPos;
		//���� ���콺 Ŀ���� ��ġ�� �����´�. 
		::GetCursorPos(&ptCursorPos);
		//���콺 ��ư�� ���� ���¿��� ���콺�� ������ ���� ���Ѵ�. 
		float cxMouseDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		// ��ŷ -> Ŭ����ǥ�� ������ǥ����� ��ȯ�� �� ����
		POINT PickingPos{ ptCursorPos };

		if (pKeyBuffer[VK_RBUTTON] & 0xF0) 
		{
			ScreenToClient(m_hWnd, &PickingPos);

			CViewport& viewport{ m_pPlayer->GetCamera()->m_Viewport };
			CCamera& camera{ *m_pPlayer->GetCamera() };

			// ��ũ�� ��ǥ�� -> ���� ��ǥ��
			XMFLOAT2 projPos{ 2.0f * PickingPos.x / (float)viewport.m_nWidth - 1.0f,
				-2.0f * PickingPos.y / (float)viewport.m_nHeight + 1.0f };
			
			// ���� ��ǥ�� -> ī�޶� ��ǥ��
			XMFLOAT3 camPos{ projPos.x / camera.m_xmf4x4Project._11, projPos.y / camera.m_xmf4x4Project._22, 1.0f };

			XMFLOAT4X4 worldMatrix{}; // ���庯ȯ ���
			XMStoreFloat4x4(&worldMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera.m_xmf4x4View)));

			// ī�޶� ��ǥ�� -> ���� ��ǥ��
			XMFLOAT3 rayDir{ camPos.x, camPos.y, camPos.z }, rayOrigin{};	// ���� ����� ������
			XMStoreFloat3(&rayOrigin, XMVector3TransformCoord(XMLoadFloat3(&rayOrigin), XMLoadFloat4x4(&worldMatrix)));
			XMStoreFloat3(&rayDir, XMVector3TransformNormal(XMLoadFloat3(&rayDir), XMLoadFloat4x4(&worldMatrix)));

			int idx{ m_pScene->CheckPicking(rayOrigin, rayDir) };
			if (idx != -1) m_pPlayer->SetPickedObject(m_pScene->GetGameObject(idx));
		}

		//���콺 Ŀ���� ��ġ�� ���콺�� �������� ��ġ�� �����Ѵ�. 
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			//���콺 �̵��� ������ �÷��̾ ȸ���Ѵ�. 
			/*cxDelta�� y-���� ȸ���� ��Ÿ���� cyDelta�� x-���� ȸ���� ��Ÿ����.
			������ ���콺 ��ư�� ������ ��� cxDelta�� z-���� ȸ���� ��Ÿ����.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) m_pPlayer->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else m_pPlayer->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
	//�÷��̾ ������ �̵��ϰ� ī�޶� �����Ѵ�. �������� ������ �ӵ� ���Ϳ� �����Ѵ�. 
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects() 
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pPlayer) m_pPlayer->Animate(fTimeElapsed); 
	if (m_pScene) m_pScene->Animate(fTimeElapsed);
	if (m_pPlayer)
		for (int i = 0; i < m_pPlayer->GetNumOfBullet(); ++i)
			m_pPlayer->GetBullet(i)->Animate(fTimeElapsed);
}

void CGameFramework::FrameAdvance()
{
	//Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ����Ѵ�. Tick(0.0f)�� ���غ���. 
	m_GameTimer.Tick(60.0f);
	ProcessInput(); 
	AnimateObjects();
	ClearFrameBuffer(RGB(255, 255, 255));
	CCamera* pCamera = m_pPlayer->GetCamera();
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera);
	//�÷��̾�(�����)�� �������Ѵ�. 
	if (m_pPlayer) m_pPlayer->Render(m_hDCFrameBuffer, pCamera);
	//�÷��̾��� �Ѿ��� �������Ѵ�.
	if (m_pPlayer)
		for (int i = 0; i < m_pPlayer->GetNumOfBullet(); ++i)
			m_pPlayer->GetBullet(i)->Render(m_hDCFrameBuffer, pCamera);

	PresentFrameBuffer();
	//���� ������ ����Ʈ�� ������ ĸ��(Ÿ��Ʋ ��)�� ����Ѵ�. 
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

	// �浹ó��
	m_pScene->ProcessCollision();
}