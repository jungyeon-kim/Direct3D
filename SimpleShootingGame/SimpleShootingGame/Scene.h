#pragma once

class CGameObject;
class CObjectsShader;
class CBulletsShader;
class CParticlesShader;
class CPlayer;

class CScene
{
protected:
    //배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
    CObjectsShader* m_pShaders{};
    int m_nShaders{};

    //파티클에 대한 셰이더이다.
    CParticlesShader* ParticlesShader{};
    int NumOfParticles{};

    //프레임워크에서 생성한 플레이어를 참조하기위한 변수이다.
    CPlayer* Player{};
    //플레이어가 가지고있는 불릿셰이더를 참조하기위한 변수이다.
    CBulletsShader* BulletsShader{};

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

    //씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다. 
    CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera *pCamera);

    void SetPlayer(CPlayer* NewPlayer);

    //충돌처리를 수행한다.
    bool IsCollided(const BoundingOrientedBox& Lhs, const BoundingOrientedBox& Rhs);
    void ProcessCollision();
};