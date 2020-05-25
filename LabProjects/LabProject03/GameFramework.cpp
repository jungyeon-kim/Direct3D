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

	//비행기 메쉬를 생성하고 플레이어 객체에 연결한다. 
	CAirplaneMesh* pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer = new CAirplanePlayer(); 
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pAirplaneMesh); 
	m_pPlayer->SetColor(RGB(0, 0, 255)); 
	m_pPlayer->SetCamera(pCamera); 
	m_pPlayer->SetPlayerBox();

	//카메라는 플레이어 객체 뒤쪽 위에서 플레이어를 바라본다. 
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));
	m_pScene = new CScene(m_pPlayer); 
	m_pScene->BuildObjects();
}

void CGameFramework::ReleaseObjects() 
{ 
	//씬 객체의 게임 객체들을 소멸하고, 씬 객체와 플레이어 객체를 소멸한다. 
	if (m_pScene) m_pScene->ReleaseObjects(); 
	if (m_pScene) delete m_pScene; 
	if (m_pPlayer) delete m_pPlayer; 
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (nMessageID) 
	{ 
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다. 
	case WM_RBUTTONDOWN: 
	case WM_LBUTTONDOWN: 
		::SetCapture(hWnd); 
		::GetCursorPos(&m_ptOldCursorPos); 
		break; 
		//마우스 캡쳐를 해제한다. 
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
	/*키보드의 상태 정보를 반환한다.
	화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로 컬 x-축), 앞/뒤(로컬 z-축)로 이동한다.
	‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로 컬 y-축)로 이동한다.*/
	if (::GetKeyboardState(pKeyBuffer))
	{
		DWORD dwDirection = 0;
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		//키 입력이 있으면 플레이어를 dwDirection 방향으로 이동한다(실제로는 속도 벡터를 변경한다). 
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.15f);
	}
	if (::GetCapture() == m_hWnd)
	{
		/*마우스를 캡쳐했으면(마우스 클릭이 일어났으면) 마우스가 얼마만큼 이동하였는 가를 계산한다.
		마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의 메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다.
		그러므로 마우스가 캡쳐된 윈도우가 현재 윈도우이면 마우스 버튼이 현재 윈도 우의 클라이언트 영역에서 눌려진 상태를
		의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플레이어를 x-축 또는 y-축으로 회전한다.*/
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다). 
		::SetCursor(NULL);
		POINT ptCursorPos;
		//현재 마우스 커서의 위치를 가져온다. 
		::GetCursorPos(&ptCursorPos);
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다. 
		float cxMouseDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		float cyMouseDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;

		// 피킹 -> 클릭좌표를 월드좌표계까지 변환한 뒤 수행
		POINT PickingPos{ ptCursorPos };

		if (pKeyBuffer[VK_RBUTTON] & 0xF0) 
		{
			ScreenToClient(m_hWnd, &PickingPos);

			CViewport& viewport{ m_pPlayer->GetCamera()->m_Viewport };
			CCamera& camera{ *m_pPlayer->GetCamera() };

			// 스크린 좌표계 -> 투영 좌표계
			XMFLOAT2 projPos{ 2.0f * PickingPos.x / (float)viewport.m_nWidth - 1.0f,
				-2.0f * PickingPos.y / (float)viewport.m_nHeight + 1.0f };
			
			// 투영 좌표계 -> 카메라 좌표계
			XMFLOAT3 camPos{ projPos.x / camera.m_xmf4x4Project._11, projPos.y / camera.m_xmf4x4Project._22, 1.0f };

			XMFLOAT4X4 worldMatrix{}; // 월드변환 행렬
			XMStoreFloat4x4(&worldMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&camera.m_xmf4x4View)));

			// 카메라 좌표계 -> 월드 좌표계
			XMFLOAT3 rayDir{ camPos.x, camPos.y, camPos.z }, rayOrigin{};	// 광선 방향과 시작점
			XMStoreFloat3(&rayOrigin, XMVector3TransformCoord(XMLoadFloat3(&rayOrigin), XMLoadFloat4x4(&worldMatrix)));
			XMStoreFloat3(&rayDir, XMVector3TransformNormal(XMLoadFloat3(&rayDir), XMLoadFloat4x4(&worldMatrix)));

			int idx{ m_pScene->CheckPicking(rayOrigin, rayDir) };
			if (idx != -1) m_pPlayer->SetPickedObject(m_pScene->GetGameObject(idx));
		}

		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다. 
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta)
		{
			//마우스 이동이 있으면 플레이어를 회전한다. 
			/*cxDelta는 y-축의 회전을 나타내고 cyDelta는 x-축의 회전을 나타낸다.
			오른쪽 마우스 버튼이 눌려진 경우 cxDelta는 z-축의 회전을 나타낸다.*/
			if (pKeyBuffer[VK_RBUTTON] & 0xF0) m_pPlayer->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else m_pPlayer->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
	//플레이어를 실제로 이동하고 카메라를 갱신한다. 마찰력의 영향을 속도 벡터에 적용한다. 
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
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다. Tick(0.0f)과 비교해보라. 
	m_GameTimer.Tick(60.0f);
	ProcessInput(); 
	AnimateObjects();
	ClearFrameBuffer(RGB(255, 255, 255));
	CCamera* pCamera = m_pPlayer->GetCamera();
	if (m_pScene) m_pScene->Render(m_hDCFrameBuffer, pCamera);
	//플레이어(비행기)를 렌더링한다. 
	if (m_pPlayer) m_pPlayer->Render(m_hDCFrameBuffer, pCamera);
	//플레이어의 총알을 렌더링한다.
	if (m_pPlayer)
		for (int i = 0; i < m_pPlayer->GetNumOfBullet(); ++i)
			m_pPlayer->GetBullet(i)->Render(m_hDCFrameBuffer, pCamera);

	PresentFrameBuffer();
	//현재 프레임 레이트를 윈도우 캡션(타이틀 바)에 출력한다. 
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);

	// 충돌처리
	m_pScene->ProcessCollision();
}