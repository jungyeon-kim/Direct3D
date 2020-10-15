#pragma once

class CShader;

class CScene
{
protected:
    // 씬은 셰이더들의 집합, 셰이더들은 게임 객체들의 집합
    CShader** Shaders{};
    int NumOfShader{};
public:
    // 루트 시그너쳐를 나타내는 인터페이스 포인터
    ID3D12RootSignature* RootSignature{};
    // 파이프라인 상태를 나타내는 인터페이스 포인터
    ID3D12PipelineState* PipelineState{};
public:
    CScene() = default;
    ~CScene() = default;

    bool ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool ProcessInput();

    void AddObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList);
    void ReleaseObjects();
    void ReleaseUploadBuffers();

    void Animate(float ElapsedTime);
    void Render(ID3D12GraphicsCommandList* CommandList);

    ID3D12RootSignature* CreateRootSignature(ID3D12Device* Device);
    ID3D12RootSignature* GetRootSignature();
};