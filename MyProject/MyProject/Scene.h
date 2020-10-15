#pragma once

class CShader;

class CScene
{
protected:
    // ���� ���̴����� ����, ���̴����� ���� ��ü���� ����
    CShader** Shaders{};
    int NumOfShader{};
public:
    // ��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� ������
    ID3D12RootSignature* RootSignature{};
    // ���������� ���¸� ��Ÿ���� �������̽� ������
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