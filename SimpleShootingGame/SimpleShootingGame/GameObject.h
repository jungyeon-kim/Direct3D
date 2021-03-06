#pragma once

class CMesh;
class CShader;
class CCamera;

class CGameObject
{
private:
    int m_nReferences{};
protected:
    XMFLOAT4X4 m_xmf4x4World{ Matrix4x4::Identity() };
    CMesh* m_pMesh{};
    CShader* m_pShader{};

    bool IsVisible{ true };
public:
    CGameObject();
    virtual ~CGameObject();

    void AddRef() { ++m_nReferences; }
    void Release() { if (--m_nReferences <= 0) delete this; }

    //상수 버퍼를 생성한다. 
    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    //상수 버퍼의 내용을 갱신한다. 
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    void ReleaseUploadBuffers();

    virtual void SetMesh(CMesh* pMesh);
    virtual void SetShader(CShader* pShader);
    virtual void Animate(float fTimeElapsed);
    virtual void OnPrepareRender();
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

    //모델 좌표계의 픽킹 광선을 생성한다. 
    void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
    XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
    //카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다. 
    int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);

    //게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다. 
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetLook();
    XMFLOAT3 GetUp();
    XMFLOAT3 GetRight();

    XMFLOAT4X4 GetWorldMatrix() const { return m_xmf4x4World; }

    //게임 객체의 위치를 설정한다. 
    void SetPosition(float x, float y, float z);
    void SetPosition(XMFLOAT3 xmf3Position);
    //게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
    void MoveStrafe(float fDistance = 1.0f);
    void MoveUp(float fDistance = 1.0f);
    void MoveForward(float fDistance = 1.0f);
    void Move(XMFLOAT3& vDirection, float fSpeed);
    //게임 객체를 회전(x-축, y-축, z-축)한다. 
    void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
    void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

    BoundingOrientedBox GetBoundingBox() const;

    bool GetVisible() const { return IsVisible; }
    void SetVisible(bool Flag) { IsVisible = Flag; }
};

class CBaseObject : public CGameObject
{
private:
    XMFLOAT3 m_xmf3MovingDirection{};
    float m_fMovingSpeed{};

    XMFLOAT3 m_xmf3RotationAxis{};
    float m_fRotationSpeed{};
public:
    CBaseObject();
    virtual ~CBaseObject();

    XMFLOAT3 GetMovingDirection() const { return m_xmf3MovingDirection; }
    XMFLOAT3 GetRotationAxis() const { return m_xmf3RotationAxis; }

    void SetMovingDirection(const XMFLOAT3& xmf3MovingDirection) 
    { 
        XMStoreFloat3(&m_xmf3MovingDirection, XMVector3Normalize(XMLoadFloat3(&xmf3MovingDirection))); 
    }
    void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
    void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
    void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
    virtual void Animate(float fTimeElapsed);
};