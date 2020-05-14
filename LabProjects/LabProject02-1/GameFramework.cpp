#include "stdafx.h"
#include "GameFramework.h"
#include "Player.h" 
#include "Scene.h"

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) 
{
	m_hInstance = hInstance; m_hWnd = hMainWnd; 
	//렌더링 화면을 생성한다. 
	BuildFrameBuffer(); 
	//플레이어와 게임 세계(씬)을 생성한다. 
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
	//카메라를 생성하고 뷰포트와 시야각(FOV)를 설정한다. 
	CCamera* pCamera = new CCamera(); 
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->SetFOVAngle(90.0f);
	//플레이어 게임 객체를 생성하고 카메라와 위치를 설정한다. 
	m_pPlayer = new CPlayer(); 
	m_pPlayer->SetCamera(pCamera); 
	m_pPlayer->SetPosition(0.0f, 3.0f, -40.0f);
	//씬 객체를 생성하고 게임 객체들을 생성한다. 
	m_pScene = new CScene(); 
	m_pScene->BuildObjects();
}

void CGameFramework::ReleaseObjects() 
{ 
	//씬 객체의 게임 객체들을 소멸하고, 씬 객체와 플레이어 객체를 소멸한다. 
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
	//사용자 입력을 처리한다. 
	ProcessInput(); 
	//게임 세계를 애니메이션(움직이게)한다. 
	AnimateObjects();
	//렌더링을 할 대상 화면(비트맵)을 지운다. 
	ClearFrameBuffer(RGB(90, 103, 224));
	//씬을 렌더링한다. 
	CCamera* pCamera = m_pPlayer->GetCamera(); 
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera);
	//렌더링을 한 화면(비트맵)을 클라이언트 영역으로 복사한다. 
	PresentFrameBuffer(); 
}
