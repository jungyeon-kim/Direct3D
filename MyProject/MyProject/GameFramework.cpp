#include "stdafx.h"
#include "GameFramework.h"
#include "Scene.h"

CGameFramework::CGameFramework()
{
	_tcscpy_s(FrameRate, _T("MyProject ("));
}

void CGameFramework::Init(HINSTANCE hInstance, HWND hMainWnd)
{
	hInstance = hInstance;
	hWnd = hMainWnd;

	// 디바이스 초기화
	CreateDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetView();
	CreateDepthStencilView();

	// 객체 생성
	AddObjects();
}

void CGameFramework::Destroy()
{
	// GPU가 모든 명령 리스트를 실행할 때 까지 기다림
	WaitForGpuComplete();

	// 객체 소멸
	ReleaseObjects();

	CloseHandle(hFenceEvent);

	for (int i = 0; i < NumOfFrameBuffer; i++) if (RenderTargetBuffers[i]) RenderTargetBuffers[i]->Release();
	if (RtvDescriptorHeap) RtvDescriptorHeap->Release();
	if (DepthStencilBuffer) DepthStencilBuffer->Release();
	if (DsvDescriptorHeap) DsvDescriptorHeap->Release();
	if (CommandAllocator) CommandAllocator->Release();
	if (CommandQueue) CommandQueue->Release();
	if (PipelineState) PipelineState->Release();
	if (CommandList) CommandList->Release();
	if (Fence) Fence->Release();
	if (SwapChain) SwapChain->Release();
	if (Device) Device->Release();
	if (Factory) Factory->Release();
}

void CGameFramework::CreateSwapChain()
{
	RECT RC;
	GetClientRect(hWnd, &RC);
	WindowWidth = RC.right - RC.left;
	WindowHeight = RC.bottom - RC.top;

	// 스왑체인 설정
	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	SwapChainDesc.BufferCount = NumOfFrameBuffer;
	SwapChainDesc.BufferDesc.Width = WindowWidth;
	SwapChainDesc.BufferDesc.Height = WindowHeight;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.SampleDesc.Count = (bIsMSAA4xEnabled) ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = (bIsMSAA4xEnabled) ? (MSAALevel - 1) : 0;
	SwapChainDesc.Windowed = TRUE;
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// 스왑체인 생성
	Factory->CreateSwapChain(CommandQueue, &SwapChainDesc, (IDXGISwapChain**)&SwapChain);
	// 스왑체인의 현재 후면버퍼 인덱스 저장
	CurFrameBufferIdx = SwapChain->GetCurrentBackBufferIndex();

	// alt + enter 허용안함
	Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	// 풀스크린 상태 여부에 따라 스왑
	BOOL bIsFullScreenState{};
	SwapChain->GetFullscreenState(&bIsFullScreenState, NULL);
	SwapChain->SetFullscreenState(!bIsFullScreenState, NULL);

	// 디스플레이 모드 설정
	DXGI_MODE_DESC DisplayMode{};
	DisplayMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DisplayMode.Width = WindowWidth;
	DisplayMode.Height = WindowHeight;
	DisplayMode.RefreshRate.Numerator = 60;
	DisplayMode.RefreshRate.Denominator = 1;
	DisplayMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DisplayMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain->ResizeTarget(&DisplayMode);

	for (int i = 0; i < NumOfFrameBuffer; ++i)
		if (RenderTargetBuffers[i])
			RenderTargetBuffers[i]->Release();

	// 스왑체인 설정
	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	SwapChain->GetDesc(&SwapChainDesc);
	SwapChain->ResizeBuffers(NumOfFrameBuffer, WindowWidth, WindowHeight, 
		SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags);

	CurFrameBufferIdx = SwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetView();
}

void CGameFramework::CreateDevice()
{
	IDXGIAdapter1* Adapter{};

	// 팩토리 생성
	CreateDXGIFactory2(0, __uuidof(IDXGIFactory4), (void**)&Factory);

	// 모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != Factory->EnumAdapters1(i, &Adapter); ++i)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc{};
		Adapter->GetDesc1(&AdapterDesc);
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&Device))) break;
	}
	// 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성
	if (!Adapter)
	{
		Factory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&Adapter);
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&Device);
	}

	// 다중 샘플링 설정
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQualityLevel{};
	MSAAQualityLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAAQualityLevel.SampleCount = 4; // Msaa4x 다중 샘플링
	MSAAQualityLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAAQualityLevel.NumQualityLevels = 0;
	Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&MSAAQualityLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// 디바이스가 지원하는 다중 샘플의 품질 수준을 확인
	MSAALevel = MSAAQualityLevel.NumQualityLevels;
	// 다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화
	bIsMSAA4xEnabled = (MSAALevel > 1) ? true : false;

	// 펜스 생성
	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&Fence);

	// 펜스와 동기화를 위한 이벤트 객체를 생성(이벤트 객체의 초기값을 FALSE)
	// 이벤트가 실행되면(Signal) 이벤트의 값을 자동적으로 FALSE가 되도록 생성
	hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// 뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast<float>(WindowWidth);
	Viewport.Height = static_cast<float>(WindowHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	// 씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정
	ScissorRect = { 0, 0, WindowWidth, WindowHeight };

	if (Adapter) Adapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	// 명령 큐 설정 및 생성
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc{};
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Device->CreateCommandQueue(&CommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&CommandQueue);

	// 명령 할당자 생성
	Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		__uuidof(ID3D12CommandAllocator), (void **)&CommandAllocator);

	// 명령 리스트 생성
	Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, NULL, 
		__uuidof(ID3D12GraphicsCommandList), (void **)&CommandList);

	// 명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 변환
	// 열려있을 때 명령을 추가하고 닫혀있을 때 명령을 실행 가능
	CommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	// Rtv 서술자 힙 설정
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
	DescriptorHeapDesc.NumDescriptors = NumOfFrameBuffer;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;
	// 렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&RtvDescriptorHeap);
	// 렌더 타겟 서술자 힙의 원소의 크기를 저장
	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Dsv 서술자 힙 설정
	DescriptorHeapDesc.NumDescriptors = 1;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	// 깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&DsvDescriptorHeap);
	// 깊이-스텐실 서술자 힙의 원소의 크기를 저장
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle{ RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// 전면버퍼는 후면버퍼가 될 수 있으므로 모든 버퍼에 대해 뷰를 생성
	for (int i = 0; i < NumOfFrameBuffer; ++i)
	{
		SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&RenderTargetBuffers[i]);
		Device->CreateRenderTargetView(RenderTargetBuffers[i], NULL, RtvCPUDescriptorHandle);
		RtvCPUDescriptorHandle.ptr += RtvDescriptorSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	// 리소스 설정
	D3D12_RESOURCE_DESC ResourceDesc{};
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	ResourceDesc.Alignment = 0;
	ResourceDesc.Width = WindowWidth;
	ResourceDesc.Height = WindowHeight;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ResourceDesc.SampleDesc.Count = (bIsMSAA4xEnabled) ? 4 : 1;
	ResourceDesc.SampleDesc.Quality = (bIsMSAA4xEnabled) ? (MSAALevel - 1) : 0;
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 힙 프로퍼티 설정
	D3D12_HEAP_PROPERTIES HeapProperties{};
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	// 클리어 값 설정
	D3D12_CLEAR_VALUE ClearValue{};
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.0f;
	ClearValue.DepthStencil.Stencil = 0;

	// 깊이-스텐실 버퍼 리소스 생성
	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue,
		__uuidof(ID3D12Resource), (void **)&DepthStencilBuffer);

	// 깊이-스텐실 버퍼 뷰 생성
	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle{ DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	Device->CreateDepthStencilView(DepthStencilBuffer, NULL, DsvCPUDescriptorHandle);
}

void CGameFramework::AddObjects()
{
	CommandList->Reset(CommandAllocator, NULL);

	// 씬 생성 및 씬 내부객체 생성
	Scene = new CScene{};
	Scene->AddObjects(Device, CommandList);

	// 씬 객체를 생성하기 위하여 필요한 그래픽 명령 리스트들을 명령 큐에 추가
	CommandList->Close();	// 명령을 실행해야 하므로 close
	ID3D12CommandList* CommandLists[]{ CommandList };
	CommandQueue->ExecuteCommandLists(1, CommandLists);

	// 명령이 모두 실행될 때까지 대기
	WaitForGpuComplete();

	// 디폴트힙에 리소스들을 생성하는 과정에서 만든 업로드버퍼들을 소멸
	if (Scene) Scene->ReleaseUploadBuffers();
}

void CGameFramework::ReleaseObjects()
{
	if (Scene) Scene->ReleaseObjects(); 
	if (Scene) { delete Scene; Scene = nullptr; }
}

void CGameFramework::ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		break;
	}
}

void CGameFramework::ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:	PostQuitMessage(0);	break;
		case VK_F9:		ChangeSwapChainState();	break;
		}
		break;
	}
}

LRESULT CALLBACK CGameFramework::ProcessWindowMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		WindowWidth = LOWORD(lParam);
		WindowHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessMouseMsg(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		ProcessKeyboardMsg(hWnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::Animate()
{
	if (Scene) Scene->Animate(Timer.GetTimeElapsed());
}

void CGameFramework::Tick()
{
	//타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산
	Timer.Tick(0.0f);

	// 인풋과 애니메이션 처리
	ProcessInput();
	Animate();

	// 명령 할당자와 명령 리스트 리셋
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator, NULL);	// open 상태로 변함

	// 뷰포트와 시저 사각형 설정
	CommandList->RSSetViewports(1, &Viewport); 
	CommandList->RSSetScissorRects(1, &ScissorRect);

	/* 현재 렌더 타겟에 대한 프레젠트가 끝나기를 기다림
	프레젠트가 끝나면 렌더 타겟 버퍼의 상태는 프레젠트 상태 (D3D12_RESOURCE_STATE_PRESENT)에서
	렌더 타겟 상태(D3D12_RESOURCE_STATE_RENDER_TARGET)로 바뀜 */
	D3D12_RESOURCE_BARRIER ResourceBarrier{};
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; 
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = RenderTargetBuffers[CurFrameBufferIdx];
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// 렌더타겟 서술자힙의 CPU 주소를 계산
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle{ RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	// 현재의 렌더 타겟에 해당하는 서술자의 CPU 주소(핸들)를 계산
	RtvCPUDescriptorHandle.ptr += CurFrameBufferIdx * RtvDescriptorSize;
	// 깊이-스텐실 서술자힙의 CPU 주소를 계산
	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle = DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// 렌더 타겟 뷰(서술자)와 깊이-스텐실 뷰(서술자)를 출력-병합 단계(OM)에 Set
	CommandList->OMSetRenderTargets(1, &RtvCPUDescriptorHandle, FALSE, &DsvCPUDescriptorHandle);

	// 원하는 값으로 렌더 타겟(뷰)와 깊이-스텐실(뷰)를 클리어
	float ClearColor[4]{ 0.0f, 0.5f, 0.3f, 1.0f };
	CommandList->ClearRenderTargetView(RtvCPUDescriptorHandle, ClearColor, 0, NULL);
	CommandList->ClearDepthStencilView(DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	// 씬을 렌더링
	if (Scene) Scene->Render(CommandList);

	/* 현재 렌더 타겟에 대한 렌더링이 끝나기를 기다림
	GPU가 렌더 타겟(버퍼)을 더 이상 사용하지 않으면 렌더 타겟의 상태는
	프리젠트 상태(D3D12_RESOURCE_STATE_PRESENT)로 바뀜 */
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; 
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; 
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// 명령 추가가 끝나면 명령 리스트 close
	CommandList->Close();

	// 명령 리스트를 명령 큐에 추가하여 실행
	ID3D12CommandList* CommandLists[]{ CommandList };
	CommandQueue->ExecuteCommandLists(1, CommandLists);

	// GPU가 모든 명령 리스트를 실행할 때까지 기다림
	WaitForGpuComplete();

	// 스왑체인을 프레젠트, 프레젠트를 하면 현재 렌더 타겟(후면버퍼)의 내용이 전면버퍼로 옮겨지고 렌더 타겟 인덱스가 바뀜
	SwapChain->Present(0, 0);
	CurFrameBufferIdx = SwapChain->GetCurrentBackBufferIndex();

	// ??
	WaitForGpuComplete();

	Timer.GetFrameRate(FrameRate + 11, 37); 
	SetWindowText(hWnd, FrameRate);
}

void CGameFramework::WaitForGpuComplete()
{
	// CPU 펜스의 값을 증가
	UINT64 FenceValue{ ++FenceValues[CurFrameBufferIdx] };
	// GPU가 펜스의 값을 인수2로 설정하는 명령을 명령 큐에 추가
	CommandQueue->Signal(Fence, FenceValue);
	//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 대기 
	if (Fence->GetCompletedValue() < FenceValue)
	{
		// 펜스의 값이 FenceValue와 같아지면 이벤트 발생
		Fence->SetEventOnCompletion(FenceValue, hFenceEvent);
		WaitForSingleObject(hFenceEvent, INFINITE);
	}
}