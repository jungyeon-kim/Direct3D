#pragma once

class CMesh;
class CShader;

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
    XMFLOAT4X4 m_xmf4x4World{};
    CMesh* m_pMesh{};
    CShader* m_pShader{};
public:
    void ReleaseUploadBuffers();
    virtual void SetMesh(CMesh* pMesh);
    virtual void SetShader(CShader* pShader);
    virtual void Animate(float fTimeElapsed);
    virtual void OnPrepareRender();
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};