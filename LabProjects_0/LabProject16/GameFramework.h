#pragma once

#include "GameTimer.h"

class CScene;
class CCamera;
class CPlayer;
class CGameObject;

class CGameFramework
{
private:
	HINSTANCE m_hInstance{};
	HWND m_hWnd{};
	
	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };
	//������ ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�. 
	CGameTimer m_GameTimer{};
	//������ ������ ����Ʈ�� �� �������� ĸ
	_TCHAR m_pszFrameRate[50]{};

	// DXGI ���丮 �������̽��� ���� ������
	IDXGIFactory4* m_pdxgiFactory{};
	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ�
	IDXGISwapChain3* m_pdxgiSwapChain{};
	// Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ�
	ID3D12Device* m_pd3dDevice{};

	bool m_bMsaa4xEnable{};

	// MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ ����
	UINT m_nMsaa4xQualityLevels{};
	// ���� ü���� �ĸ� ������ ����
	static const UINT m_nSwapChainBuffers{ 2 };
	// ���� ���� ü���� �ĸ� ���� �ε���
	UINT m_nSwapChainBufferIndex{};

	// ���� Ÿ�� ����
	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers]{};
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap{};
	// ���� Ÿ�� ������ ������ ũ��
	UINT m_nRtvDescriptorIncrementSize{};

	// ����-���ٽ� ����
	ID3D12Resource* m_pd3dDepthStencilBuffer{};
	// ������ �� �������̽� ������
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap{};
	// ����-���ٽ� ������ ������ ũ��
	UINT m_nDsvDescriptorIncrementSize{};

	// ��� ť
	ID3D12CommandQueue* m_pd3dCommandQueue{};
	// ��� �Ҵ���
	ID3D12CommandAllocator* m_pd3dCommandAllocator{};
	// ��� ����Ʈ �������̽� ������
	ID3D12GraphicsCommandList* m_pd3dCommandList{};

	// �׷��Ƚ� ���������� ���� ��ü�� ���� �������̽� ������
	ID3D12PipelineState* m_pd3dPipelineState{};

	// �潺 �������̽� ������
	ID3D12Fence* m_pd3dFence{};
	// �潺�� ��
	UINT64 m_nFenceValues[m_nSwapChainBuffers]{};
	// �潺�̺�Ʈ �ڵ�
	HANDLE m_hFenceEvent{};

	CGameObject* m_pSelectedObject{};

	CScene* m_pScene{};
public:
	CCamera* m_pCamera{};
	CPlayer* m_pPlayer{};

	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�. 
	POINT m_ptOldCursorPos; 
public:
	CGameFramework();
	~CGameFramework() = default;

	// �����ӿ�ũ �ʱ�ȭ (�����찡 �����Ǹ� ȣ��)
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	// �����ӿ�ũ ����
	void OnDestroy();

	// ���� ü�� ����
	void CreateSwapChain();
	// ���� ü�� ���� ����
	void ChangeSwapChainState();
	// ����̽� ����
	void CreateRtvAndDsvDescriptorHeaps();
	// ������ �� ����
	void CreateDirect3DDevice();
	// ��� ť, �Ҵ���, ����Ʈ ����
	void CreateCommandQueueAndList();

	// ���� Ÿ�� �� ����
	void CreateRenderTargetView();
	// ����-���ٽ� �� ����
	void CreateDepthStencilView();

	// ������ �޽�, ���� ��ü ����
	void BuildObjects();
	// ������ �޽�, ���� ��ü �Ҹ�
	void ReleaseObjects();

	// ����� �Է� ����
	void ProcessInput();
	// ��ŷ�� ��ü ����
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	// �ִϸ��̼� ����
	void AnimateObjects();
	// ������ ����
	void FrameAdvance();

	// CPU & GPU ����ȭ
	void WaitForGpuComplete();

	// ���콺 �޼��� ó��
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// Ű���� �޼��� ó��
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// �޼��� ó��
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void MoveToNextFrame();
};
