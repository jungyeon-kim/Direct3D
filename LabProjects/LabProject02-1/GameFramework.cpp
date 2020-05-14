#include "stdafx.h"
#include "GameFramework.h"
#include "Player.h" 
#include "Scene.h"

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) 
{
	m_hInstance = hInstance; m_hWnd = hMainWnd; 
	//������ ȭ���� �����Ѵ�. 
	BuildFrameBuffer(); 
	//�÷��̾�� ���� ����(��)�� �����Ѵ�. 
	BuildObjects(); 
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
	::DeleteObject(hPen); ::DeleteObject(hBrush); 
}

void CGameFramework::PresentFrameBuffer() 
{ 
	HDC hDC = ::GetDC(m_hWnd); 
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right - m_rcClient.left, 
		m_rcClient.bottom - m_rcClient.top, m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY); 
	::ReleaseDC(m_hWnd, hDC); 
}

void CGameFramework::BuildObjects() 
{ 
	//ī�޶� �����ϰ� ����Ʈ�� �þ߰�(FOV)�� �����Ѵ�. 
	CCamera* pCamera = new CCamera(); 
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->SetFOVAngle(90.0f);
	//�÷��̾� ���� ��ü�� �����ϰ� ī�޶�� ��ġ�� �����Ѵ�. 
	m_pPlayer = new CPlayer(); 
	m_pPlayer->SetCamera(pCamera); 
	m_pPlayer->SetPosition(0.0f, 3.0f, -40.0f);
	//�� ��ü�� �����ϰ� ���� ��ü���� �����Ѵ�. 
	m_pScene = new CScene(); 
	m_pScene->BuildObjects();
}

void CGameFramework::ReleaseObjects() 
{ 
	//�� ��ü�� ���� ��ü���� �Ҹ��ϰ�, �� ��ü�� �÷��̾� ��ü�� �Ҹ��Ѵ�. 
	if (m_pScene) m_pScene->ReleaseObjects(); 
	if (m_pScene) { delete m_pScene; m_pScene = nullptr; };
	if (m_pPlayer) { delete m_pPlayer; m_pPlayer = nullptr; };
}

void CGameFramework::ProcessInput() 
{
	static UCHAR pKeyBuffer[256]; 
	if (::GetKeyboardState(pKeyBuffer)) 
	{
		float cxKeyDelta = 0.0f, cyKeyDelta = 0.0f, czKeyDelta = 0.0f; 

		if (pKeyBuffer[VK_UP] & 0xF0) czKeyDelta = +0.125f; 
		if (pKeyBuffer[VK_DOWN] & 0xF0) czKeyDelta = -0.125f; 
		if (pKeyBuffer[VK_LEFT] & 0xF0) cxKeyDelta = -0.125f; 
		if (pKeyBuffer[VK_RIGHT] & 0xF0) cxKeyDelta = +0.125f; 
		if (pKeyBuffer[VK_PRIOR] & 0xF0) cyKeyDelta = +0.125f; 
		if (pKeyBuffer[VK_NEXT] & 0xF0) cyKeyDelta = -0.125f;
		m_pPlayer->Move(cxKeyDelta, cyKeyDelta, czKeyDelta);
	}
}

void CGameFramework::AnimateObjects() 
{ 
	if (m_pScene) m_pScene->Animate(1.0f / 60.0f); 
}

void CGameFramework::FrameAdvance() 
{    
	//����� �Է��� ó���Ѵ�. 
	ProcessInput(); 
	//���� ���踦 �ִϸ��̼�(�����̰�)�Ѵ�. 
	AnimateObjects();
	//�������� �� ��� ȭ��(��Ʈ��)�� �����. 
	ClearFrameBuffer(RGB(90, 103, 224));
	//���� �������Ѵ�. 
	CCamera* pCamera = m_pPlayer->GetCamera(); 
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera);
	//�������� �� ȭ��(��Ʈ��)�� Ŭ���̾�Ʈ �������� �����Ѵ�. 
	PresentFrameBuffer(); 
}
