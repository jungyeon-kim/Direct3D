#pragma once

class CScene
{
protected:
    //씬은 게임 객체들의 집합이다. 게임 객체는 셰이더를 포함한다. 
    class CGameObject** m_ppObjects{};
    int m_nObjects{};

    ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
public:
    CScene();
    ~CScene();

    //씬에서 마우스와 키보드 메시지를 처리한다. 
    bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool ProcessInput(UCHAR* pKeysBuffer);

    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseObjects();

    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList, class CCamera* pCamera);

    void ReleaseUploadBuffers();

    //그래픽 루트 시그너쳐를 생성한다. 
    ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
    ID3D12RootSignature* GetGraphicsRootSignature();
};