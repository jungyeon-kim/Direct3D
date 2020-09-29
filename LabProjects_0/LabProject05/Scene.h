#pragma once

class CScene
{
public:
    // 루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
    ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
    // 파이프라인 상태를 나타내는 인터페이스 포인터이다. 
    ID3D12PipelineState* m_pd3dPipelineState{};
public:
    CScene() = default;
    ~CScene() = default;

    bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

    void BuildObjects(ID3D12Device* pd3dDevice);
    void ReleaseObjects();
    bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};