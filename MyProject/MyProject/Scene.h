#pragma once

class CShader;

class CScene
{
public:
    // ��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
    ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
    // ���������� ���¸� ��Ÿ���� �������̽� �������̴�. 
    ID3D12PipelineState* m_pd3dPipelineState{};
protected:
    //���� ���̴����� �����̴�. ���̴����� ���� ��ü���� �����̴�. 
    CShader** m_ppShaders{};
    int m_nShaders{};
public:
    CScene() = default;
    ~CScene() = default;

    bool ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

    void AddObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseObjects();
    bool ProcessInput();
    void Animate(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);

    void ReleaseUploadBuffers();

    //�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
    ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
    ID3D12RootSignature* GetGraphicsRootSignature();
};