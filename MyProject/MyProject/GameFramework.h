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

	// ���� ���ø� Ȱ��ȭ ����
	bool bIsMSAA4xEnabled{};
	// ���� ���ø� ����
	UINT MSAALevel{};

	// �ĸ� ���� ���� (���� ���� 1�� ����)
	static const UINT NumOfFrameBuffer{ 2 };
	// ���� ���� ü���� �ĸ� ���� �ε���
	UINT CurFrameBufferIdx{};

	// ����Ÿ�� ����
	ID3D12Resource* RenderTargetBuffers[NumOfFrameBuffer]{};
	// ����Ÿ�� ��������
	ID3D12DescriptorHeap* RtvDescriptorHeap{};
	// ����Ÿ�� ������ ũ��
	UINT RtvDescriptorSize{};

	// ����-���ٽ� ����
	ID3D12Resource* DepthStencilBuffer{};
	// ����-���ٽ� ��������
	ID3D12DescriptorHeap* DsvDescriptorHeap{};
	// ����-���ٽ� ������ ũ��
	UINT DsvDescriptorSize{};

	// ��� ť
	ID3D12CommandQueue* CommandQueue{};
	// ��� �Ҵ���
	ID3D12CommandAllocator* CommandAllocator{};
	// ��� ����Ʈ
	ID3D12GraphicsCommandList* CommandList{};

	// �׷��Ƚ� ���������� ����
	ID3D12PipelineState* PipelineState{};

	// �潺
	ID3D12Fence* Fence{};
	// �潺 ��
	UINT64 FenceValues[NumOfFrameBuffer];
	// �潺�̺�Ʈ �ڵ�
	HANDLE hFenceEvent{};

	// ����Ʈ
	D3D12_VIEWPORT Viewport{};
	// ���� �簢��
	D3D12_RECT ScissorRect{};

	class CScene* Scene{};
public:
	CGameFramework();
	~CGameFramework() = default;

	// �����ӿ�ũ �ʱ�ȭ (�����찡 �����Ǹ� ȣ��)
	void Init(HINSTANCE hInstance, HWND hMainWnd);
	// �����ӿ�ũ ����
	void Destroy();

	// ����̽� ����
	void CreateDevice();
	// ��� ť, �Ҵ���, ����Ʈ ����
	void CreateCommandQueueAndList();
	// ���� ü�� ����
	void CreateSwapChain();
	// ����Ÿ�� ��, ����-���ٽ� �� �������� ����
	void CreateRtvAndDsvDescriptorHeaps();
	// ���� Ÿ�� �� ����
	void CreateRenderTargetView();
	// ����-���ٽ� �� ����
	void CreateDepthStencilView();

	// ���� ü�� ���� ����
	void ChangeSwapChainState();

	// ������ �޽�, ���� ��ü ����
	void AddObjects();
	// ������ �޽�, ���� ��ü �Ҹ�
	void ReleaseObjects();

	// ����� �Է� ����
	void ProcessInput();
	// �ִϸ��̼� ����
	void Animate();
	// ƽ
	void Tick();

	// CPU & GPU ����ȭ
	void WaitForGpuComplete();

	// ���콺 �޼��� ó��
	void ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// Ű���� �޼��� ó��
	void ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �޼��� ó��
	LRESULT CALLBACK ProcessWindowMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
