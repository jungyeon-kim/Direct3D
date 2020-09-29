#pragma once

#include "GameTimer.h"

class CGameFramework
{
private:
	HINSTANCE hInstance{};
	HWND hWnd{};
	
	int WindowWidth{ WINDOW_WIDTH };
	int WindowHeight{ WINDOW_HEIGHT };

	CGameTimer Timer{};
	_TCHAR FrameRate[50]{};

	ID3D12Device* Device{};
	IDXGIFactory4* Factory{};
	IDXGISwapChain3* SwapChain{};

	// 다중 샘플링 활성화 여부
	bool bIsMSAA4xEnabled{};
	// 다중 샘플링 레벨
	UINT MSAALevel{};

	// 후면 버퍼 개수 (전면 버퍼 1개 포함)
	static const UINT NumOfFrameBuffer{ 2 };
	// 현재 스왑 체인의 후면 버퍼 인덱스
	UINT CurFrameBufferIdx{};

	// 렌더타겟 버퍼
	ID3D12Resource* RenderTargetBuffers[NumOfFrameBuffer]{};
	// 렌더타겟 서술자힙
	ID3D12DescriptorHeap* RtvDescriptorHeap{};
	// 렌더타겟 서술자 크기
	UINT RtvDescriptorSize{};

	// 깊이-스텐실 버퍼
	ID3D12Resource* DepthStencilBuffer{};
	// 깊이-스텐실 서술자힙
	ID3D12DescriptorHeap* DsvDescriptorHeap{};
	// 깊이-스텐실 서술자 크기
	UINT DsvDescriptorSize{};

	// 명령 큐
	ID3D12CommandQueue* CommandQueue{};
	// 명령 할당자
	ID3D12CommandAllocator* CommandAllocator{};
	// 명령 리스트
	ID3D12GraphicsCommandList* CommandList{};

	// 그래픽스 파이프라인 상태
	ID3D12PipelineState* PipelineState{};

	// 펜스
	ID3D12Fence* Fence{};
	// 펜스 값
	UINT64 FenceValues[NumOfFrameBuffer];
	// 펜스이벤트 핸들
	HANDLE hFenceEvent{};

	// 뷰포트
	D3D12_VIEWPORT Viewport{};
	// 씨저 사각형
	D3D12_RECT ScissorRect{};

	class CScene* Scene{};
public:
	CGameFramework();
	~CGameFramework() = default;

	// 프레임워크 초기화 (윈도우가 생성되면 호출)
	void Init(HINSTANCE hInstance, HWND hMainWnd);
	// 프레임워크 종료
	void Destroy();

	// 디바이스 생성
	void CreateDevice();
	// 명령 큐, 할당자, 리스트 생성
	void CreateCommandQueueAndList();
	// 스왑 체인 생성
	void CreateSwapChain();
	// 렌더타겟 뷰, 깊이-스텐실 뷰 서술자힙 생성
	void CreateRtvAndDsvDescriptorHeaps();
	// 렌더 타겟 뷰 생성
	void CreateRenderTargetView();
	// 깊이-스텐실 뷰 생성
	void CreateDepthStencilView();

	// 스왑 체인 상태 변경
	void ChangeSwapChainState();

	// 렌더링 메쉬, 게임 객체 생성
	void AddObjects();
	// 렌더링 메쉬, 게임 객체 소멸
	void ReleaseObjects();

	// 사용자 입력 구성
	void ProcessInput();
	// 애니메이션 구성
	void Animate();
	// 틱
	void Tick();

	// CPU & GPU 동기화
	void WaitForGpuComplete();

	// 마우스 메세지 처리
	void ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 키보드 메세지 처리
	void ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 메세지 처리
	LRESULT CALLBACK ProcessWindowMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
