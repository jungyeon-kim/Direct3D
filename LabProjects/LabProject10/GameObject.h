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
public:
    CGameObject();
    virtual ~CGameObject();

    void AddRef() { ++m_nReferences; }
    void Release() { if (--m_nReferences <= 0) delete this; }

    //��� ���۸� �����Ѵ�. 
    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    //��� ������ ������ �����Ѵ�. 
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    void ReleaseUploadBuffers();

    virtual void SetMesh(CMesh* pMesh);
    virtual void SetShader(CShader* pShader);
    virtual void Animate(float fTimeElapsed);
    virtual void OnPrepareRender();
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

    //���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
    XMFLOAT3 GetPosition();
    XMFLOAT3 GetLook();
    XMFLOAT3 GetUp();
    XMFLOAT3 GetRight();
    //���� ��ü�� ��ġ�� �����Ѵ�. 
    void SetPosition(float x, float y, float z);
    void SetPosition(XMFLOAT3 xmf3Position);
    //���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
    void MoveStrafe(float fDistance = 1.0f);
    void MoveUp(float fDistance = 1.0f);
    void MoveForward(float fDistance = 1.0f);
    //���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
    void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
    void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
};

class CRotatingObject : public CGameObject
{
private:
    XMFLOAT3 m_xmf3RotationAxis{};
    float m_fRotationSpeed{};
public:
    CRotatingObject();
    virtual ~CRotatingObject();

    void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
    void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
    virtual void Animate(float fTimeElapsed);
};