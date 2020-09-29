#pragma once

class CGameFramework
{
public: 
	CGameFramework() {} 
	~CGameFramework() {}
private: 
	//윈도우 응용프로그램의 인스턴스 핸들과 주 윈도우 핸들이다. 
	HINSTANCE m_hInstance = NULL; 
	HWND m_hWnd = NULL;
	//주 윈도우의 클라이언트 사각형 영역이다. 
	RECT m_rcClient{};
	//렌더링의 대상이 되는 화면에 해당하는 비트맵과 비트맵 디바이스 컨텍스트(Device Context)이다. 
	HDC m_hDCFrameBuffer = NULL; 
	HBITMAP m_hBitmapFrameBuffer = NULL; 
	HBITMAP m_hBitmapSelect = NULL;
	//플레이어 객체이다. 
	class CPlayer* m_pPlayer = nullptr; 
	//게임 객체들을 포함하는 씬(게임 세계) 클래스이다. 
	class CScene* m_pScene = nullptr;
public: 
	//프레임워크를 생성하는 함수이다(주 윈도우가 생성되면 호출된다). 
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd); 
	//프레임워크를 소멸하는 함수이다(응용프로그램이 종료되면 호출된다). 
	void OnDestroy();
	//게임 세계를 렌더링할 비트맵 표면을 생성하고, 지우고, 클라이언트 영역으로 복사한다. 
	void BuildFrameBuffer(); 
	void ClearFrameBuffer(DWORD dwColor); 
	void PresentFrameBuffer();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects(); 
	void ReleaseObjects();
	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput(); 
	void AnimateObjects();
	void FrameAdvance();
};

