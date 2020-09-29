#pragma once

class CMesh;
class CShader;
class CCamera;

class CGameObject
{
public:
    CGameObject();
    virtual ~CGameObject();
private:
    int m_nReferences{};
public:
    void AddRef() { ++m_nReferences; }
    void Release() { if (--m_nReferences <= 0) delete this; }
protected:
    XMFLOAT4X4 m_xmf4x4World{ Matrix4x4::Identity() };
    CMesh* m_pMesh{};
    CShader* m_pShader{};
public:
    void ReleaseUploadBuffers();
    virtual void SetMesh(CMesh* pMesh);
    virtual void SetShader(CShader* pShader);
    virtual void Animate(float fTimeElapsed);
    virtual void OnPrepareRender();
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

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