#pragma once

class CObjectsShader;
class CHeightMapTerrain;

class CScene
{
protected:
    //��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
    CObjectsShader* m_pShaders{};
    int m_nShaders{};

    CHeightMapTerrain* m_pTerrain{};

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

    CHeightMapTerrain* GetTerrain() { return m_pTerrain; }
};