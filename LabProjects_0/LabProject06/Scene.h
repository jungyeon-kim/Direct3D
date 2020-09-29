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

    bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseObjects();
    bool ProcessInput();
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);

    void ReleaseUploadBuffers();

    //�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
    ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
    ID3D12RootSignature* GetGraphicsRootSignature();
};