#pragma once

class CShader;

class CScene
{
public:
    // 루트 시그너쳐를 나타내는 인터페이스 포인터이다. 
    ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
    // 파이프라인 상태를 나타내는 인터페이스 포인터이다. 
    ID3D12PipelineState* m_pd3dPipelineState{};
protected:
    //씬은 셰이더들의 집합이다. 셰이더들은 게임 객체들의 집합이다. 
    CShader** m_ppShaders{};
    int m_nShaders{};
public:
    CScene() = default;
    ~CScene() = default;

    bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseObjects();
    bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);

    void ReleaseUploadBuffers();

    //그래픽 루트 시그너쳐를 생성한다. 
    ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
    ID3D12RootSignature* GetGraphicsRootSignature();
};