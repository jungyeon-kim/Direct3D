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

	// ����̽� �ʱ�ȭ
	CreateDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetView();
	CreateDepthStencilView();

	// ��ü ����
	AddObjects();
}

void CGameFramework::Destroy()
{
	// GPU�� ��� ��� ����Ʈ�� ������ �� ���� ��ٸ�
	WaitForGpuComplete();

	// ��ü �Ҹ�
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

	// ����ü�� ����
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
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� ����
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// ����ü�� ����
	Factory->CreateSwapChain(CommandQueue, &SwapChainDesc, (IDXGISwapChain**)&SwapChain);
	// ����ü���� ���� �ĸ���� �ε��� ����
	CurFrameBufferIdx = SwapChain->GetCurrentBackBufferIndex();

	// alt + enter ������
	Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	// Ǯ��ũ�� ���� ���ο� ���� ����
	BOOL bIsFullScreenState{};
	SwapChain->GetFullscreenState(&bIsFullScreenState, NULL);
	SwapChain->SetFullscreenState(!bIsFullScreenState, NULL);

	// ���÷��� ��� ����
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

	// ����ü�� ����
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

	// ���丮 ����
	CreateDXGIFactory2(0, __uuidof(IDXGIFactory4), (void**)&Factory);

	// ��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ����
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != Factory->EnumAdapters1(i, &Adapter); ++i)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc{};
		Adapter->GetDesc1(&AdapterDesc);
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&Device))) break;
	}
	// Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� ����
	if (!Adapter)
	{
		Factory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&Adapter);
		D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&Device);
	}

	// ���� ���ø� ����
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS MSAAQualityLevel{};
	MSAAQualityLevel.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	MSAAQualityLevel.SampleCount = 4; // Msaa4x ���� ���ø�
	MSAAQualityLevel.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	MSAAQualityLevel.NumQualityLevels = 0;
	Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&MSAAQualityLevel, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	// ����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ��
	MSAALevel = MSAAQualityLevel.NumQualityLevels;
	// ���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ
	bIsMSAA4xEnabled = (MSAALevel > 1) ? true : false;

	// �潺 ����
	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&Fence);

	// �潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� ����(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE)
	// �̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� ����
	hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// ����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� ����
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = static_cast<float>(WindowWidth);
	Viewport.Height = static_cast<float>(WindowHeight);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	// ���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� ����
	ScissorRect = { 0, 0, WindowWidth, WindowHeight };

	if (Adapter) Adapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	// ��� ť ���� �� ����
	D3D12_COMMAND_QUEUE_DESC CommandQueueDesc{};
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Device->CreateCommandQueue(&CommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&CommandQueue);

	// ��� �Ҵ��� ����
	Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		__uuidof(ID3D12CommandAllocator), (void **)&CommandAllocator);

	// ��� ����Ʈ ����
	Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, NULL, 
		__uuidof(ID3D12GraphicsCommandList), (void **)&CommandList);

	// ��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� ��ȯ
	// �������� �� ����� �߰��ϰ� �������� �� ����� ���� ����
	CommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	// Rtv ������ �� ����
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
	DescriptorHeapDesc.NumDescriptors = NumOfFrameBuffer;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DescriptorHeapDesc.NodeMask = 0;
	// ���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� ����
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&RtvDescriptorHeap);
	// ���� Ÿ�� ������ ���� ������ ũ�⸦ ����
	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Dsv ������ �� ����
	DescriptorHeapDesc.NumDescriptors = 1;
	DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	// ����-���ٽ� ������ ��(�������� ������ 1)�� ����
	Device->CreateDescriptorHeap(&DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&DsvDescriptorHeap);
	// ����-���ٽ� ������ ���� ������ ũ�⸦ ����
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle{ RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// ������۴� �ĸ���۰� �� �� �����Ƿ� ��� ���ۿ� ���� �並 ����
	for (int i = 0; i < NumOfFrameBuffer; ++i)
	{
		SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&RenderTargetBuffers[i]);
		Device->CreateRenderTargetView(RenderTargetBuffers[i], NULL, RtvCPUDescriptorHandle);
		RtvCPUDescriptorHandle.ptr += RtvDescriptorSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	// ���ҽ� ����
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

	// �� ������Ƽ ����
	D3D12_HEAP_PROPERTIES HeapProperties{};
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 1;
	HeapProperties.VisibleNodeMask = 1;

	// Ŭ���� �� ����
	D3D12_CLEAR_VALUE ClearValue{};
	ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ClearValue.DepthStencil.Depth = 1.0f;
	ClearValue.DepthStencil.Stencil = 0;

	// ����-���ٽ� ���� ���ҽ� ����
	Device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue,
		__uuidof(ID3D12Resource), (void **)&DepthStencilBuffer);

	// ����-���ٽ� ���� �� ����
	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle{ DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	Device->CreateDepthStencilView(DepthStencilBuffer, NULL, DsvCPUDescriptorHandle);
}

void CGameFramework::AddObjects()
{
	CommandList->Reset(CommandAllocator, NULL);

	// �� ���� �� �� ���ΰ�ü ����
	Scene = new CScene{};
	Scene->AddObjects(Device, CommandList);

	// �� ��ü�� �����ϱ� ���Ͽ� �ʿ��� �׷��� ��� ����Ʈ���� ��� ť�� �߰�
	CommandList->Close();	// ����� �����ؾ� �ϹǷ� close
	ID3D12CommandList* CommandLists[]{ CommandList };
	CommandQueue->ExecuteCommandLists(1, CommandLists);

	// ����� ��� ����� ������ ���
	WaitForGpuComplete();

	// ����Ʈ���� ���ҽ����� �����ϴ� �������� ���� ���ε���۵��� �Ҹ�
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
	//Ÿ�̸��� �ð��� ���ŵǵ��� �ϰ� ������ ����Ʈ�� ���
	Timer.Tick(0.0f);

	// ��ǲ�� �ִϸ��̼� ó��
	ProcessInput();
	Animate();

	// ��� �Ҵ��ڿ� ��� ����Ʈ ����
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator, NULL);	// open ���·� ����

	// ����Ʈ�� ���� �簢�� ����
	CommandList->RSSetViewports(1, &Viewport); 
	CommandList->RSSetScissorRects(1, &ScissorRect);

	/* ���� ���� Ÿ�ٿ� ���� ������Ʈ�� �����⸦ ��ٸ�
	������Ʈ�� ������ ���� Ÿ�� ������ ���´� ������Ʈ ���� (D3D12_RESOURCE_STATE_PRESENT)����
	���� Ÿ�� ����(D3D12_RESOURCE_STATE_RENDER_TARGET)�� �ٲ� */
	D3D12_RESOURCE_BARRIER ResourceBarrier{};
	ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; 
	ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.pResource = RenderTargetBuffers[CurFrameBufferIdx];
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// ����Ÿ�� ���������� CPU �ּҸ� ���
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCPUDescriptorHandle{ RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	// ������ ���� Ÿ�ٿ� �ش��ϴ� �������� CPU �ּ�(�ڵ�)�� ���
	RtvCPUDescriptorHandle.ptr += CurFrameBufferIdx * RtvDescriptorSize;
	// ����-���ٽ� ���������� CPU �ּҸ� ���
	D3D12_CPU_DESCRIPTOR_HANDLE DsvCPUDescriptorHandle = DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// ���� Ÿ�� ��(������)�� ����-���ٽ� ��(������)�� ���-���� �ܰ�(OM)�� Set
	CommandList->OMSetRenderTargets(1, &RtvCPUDescriptorHandle, FALSE, &DsvCPUDescriptorHandle);

	// ���ϴ� ������ ���� Ÿ��(��)�� ����-���ٽ�(��)�� Ŭ����
	float ClearColor[4]{ 0.0f, 0.5f, 0.3f, 1.0f };
	CommandList->ClearRenderTargetView(RtvCPUDescriptorHandle, ClearColor, 0, NULL);
	CommandList->ClearDepthStencilView(DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	// ���� ������
	if (Scene) Scene->Render(CommandList);

	/* ���� ���� Ÿ�ٿ� ���� �������� �����⸦ ��ٸ�
	GPU�� ���� Ÿ��(����)�� �� �̻� ������� ������ ���� Ÿ���� ���´�
	������Ʈ ����(D3D12_RESOURCE_STATE_PRESENT)�� �ٲ� */
	ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; 
	ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; 
	CommandList->ResourceBarrier(1, &ResourceBarrier);

	// ��� �߰��� ������ ��� ����Ʈ close
	CommandList->Close();

	// ��� ����Ʈ�� ��� ť�� �߰��Ͽ� ����
	ID3D12CommandList* CommandLists[]{ CommandList };
	CommandQueue->ExecuteCommandLists(1, CommandLists);

	// GPU�� ��� ��� ����Ʈ�� ������ ������ ��ٸ�
	WaitForGpuComplete();

	// ����ü���� ������Ʈ, ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� �ε����� �ٲ�
	SwapChain->Present(0, 0);
	CurFrameBufferIdx = SwapChain->GetCurrentBackBufferIndex();

	// ??
	WaitForGpuComplete();

	Timer.GetFrameRate(FrameRate + 11, 37); 
	SetWindowText(hWnd, FrameRate);
}

void CGameFramework::WaitForGpuComplete()
{
	// CPU �潺�� ���� ����
	UINT64 FenceValue{ ++FenceValues[CurFrameBufferIdx] };
	// GPU�� �潺�� ���� �μ�2�� �����ϴ� ����� ��� ť�� �߰�
	CommandQueue->Signal(Fence, FenceValue);
	//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��� 
	if (Fence->GetCompletedValue() < FenceValue)
	{
		// �潺�� ���� FenceValue�� �������� �̺�Ʈ �߻�
		Fence->SetEventOnCompletion(FenceValue, hFenceEvent);
		WaitForSingleObject(hFenceEvent, INFINITE);
	}
}