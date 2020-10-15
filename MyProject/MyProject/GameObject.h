#pragma once

class CMesh;
class CShader;

class CGameObject
{
private:
    int RefCount{};
protected:
    XMFLOAT4X4 WorldMatrix{};
    CMesh* Mesh{};
    CShader* Shader{};
public:
    CGameObject();
    virtual ~CGameObject();

    void AddRef() { ++RefCount; }
    void Release() { if (--RefCount <= 0) delete this; }
    void ReleaseUploadBuffers();

    virtual void Animate(float ElapsedTime);
    virtual void OnPrepareRender();
    virtual void Render(ID3D12GraphicsCommandList* CommandList);
    virtual void SetMesh(CMesh* NewMesh);
    virtual void SetShader(CShader* NewShader);
};