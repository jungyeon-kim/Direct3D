#pragma once

class CGameObject;
class CCubeShader;

class CScene
{
protected:
    //��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
    CCubeShader* m_pShaders{};
    int m_nShaders{};

    ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
public:
    CScene();
    ~CScene();

    //������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
    bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    bool ProcessInput(UCHAR* pKeysBuffer);

    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void ReleaseObjects();

    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList* pd3dCommandList, class CCamera* pCamera);

    void ReleaseUploadBuffers();

    //�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
    ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
    ID3D12RootSignature* GetGraphicsRootSignature();

    //���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�. 
    CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);
};