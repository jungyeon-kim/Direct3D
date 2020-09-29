#pragma once

#include "GameTimer.h"

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;
	//다음은 게임 프레임워크에서 사용할 타이머이다. 
	CGameTimer m_GameTimer;
	//다음은 프레임 레이트를 주 윈도우의 캡
	_TCHAR m_pszFrameRate[50];

	// DXGI 팩토리 인터페이스에 대한 포인터
	IDXGIFactory4 *m_pdxgiFactory;
	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요
	IDXGISwapChain3 *m_pdxgiSwapChain;
	// Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요
	ID3D12Device *m_pd3dDevice;

	bool m_bMsaa4xEnable = false;

	// MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정
	UINT m_nMsaa4xQualityLevels = 0;
	// 스왑 체인의 후면 버퍼의 개수
	static const UINT m_nSwapChainBuffers = 2;
	// 현재 스왑 체인의 후면 버퍼 인덱스
	UINT m_nSwapChainBufferIndex;

	// 렌더 타겟 버퍼
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	// 렌더 타겟 서술자 원소의 크기
	UINT m_nRtvDescriptorIncrementSize;

	// 깊이-스텐실 버퍼
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	// 서술자 힙 인터페이스 포인터
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	// 깊이-스텐실 서술자 원소의 크기
	UINT m_nDsvDescriptorIncrementSize;

	// 명령 큐
	ID3D12CommandQueue *m_pd3dCommandQueue;
	// 명령 할당자
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	// 명령 리스트 인터페이스 포인터
	ID3D12GraphicsCommandList *m_pd3dCommandList;

	// 그래픽스 파이프라인 상태 객체에 대한 인터페이스 포인터
	ID3D12PipelineState *m_pd3dPipelineState;

	// 펜스 인터페이스 포인터
	ID3D12Fence *m_pd3dFence;
	// 펜스의 값
	UINT64 m_nFenceValue;
	// 펜스이벤트 핸들
	HANDLE m_hFenceEvent;

	// 뷰포트
	D3D12_VIEWPORT m_d3dViewport;
	// 씨저 사각형
	D3D12_RECT m_d3dScissorRect;
public:
	CGameFramework();
	~CGameFramework();

	// 프레임워크 초기화 (윈도우가 생성되면 호출)
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// 프레임워크 종료
	void OnDestroy();

	// 스왑 체인 생성
	void CreateSwapChain();
	// 디바이스 생성
	void CreateRtvAndDsvDescriptorHeaps();
	// 서술자 힙 생성
	void CreateDirect3DDevice();
	// 명령 큐, 할당자, 리스트 생성
	void CreateCommandQueueAndList();

	// 렌더 타겟 뷰 생성
	void CreateRenderTargetView();
	// 깊이-스텐실 뷰 생성
	void CreateDepthStencilView();

	// 렌더링 메쉬, 게임 객체 생성
	void BuildObjects();
	// 렌더링 메쉬, 게임 객체 소멸
	void ReleaseObjects();

	// 사용자 입력 구성
	void ProcessInput();
	// 애니메이션 구성
	void AnimateObjects();
	// 렌더링 구성
	void FrameAdvance();

	// CPU & GPU 동기화
	void WaitForGpuComplete();

	// 마우스 메세지 처리
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 키보드 메세지 처리
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 메세지 처리
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
