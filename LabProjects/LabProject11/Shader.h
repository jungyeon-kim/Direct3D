#pragma once

class CGameObject;
class CCamera;

//���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ����ü(��� ����)�̴�. 
struct CB_GAMEOBJECT_INFO
{
    XMFLOAT4X4 m_xmf4x4World;
};

//���̴� �ҽ� �ڵ带 �������ϰ� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 
class CShader
{
private:
    int m_nReferences{};
protected:
    //���������� ���� ��ü���� ����Ʈ(�迭)�̴�. 
    ID3D12PipelineState** m_ppd3dPipelineStates{};
    int m_nPipelineStates{};
public:
    CShader();
    virtual ~CShader();

    void AddRef() { m_nReferences++; }
    void Release() { if (--m_nReferences <= 0) delete this; }

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
    virtual D3D12_BLEND_DESC CreateBlendState();
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
    D3D12_SHADER_BYTECODE CompileShaderFromFile(LPCWSTR pszFileName, LPCSTR pszShaderName,
        LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

    virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};

class CPlayerShader : public CShader
{
public:
    CPlayerShader();
    virtual ~CPlayerShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
};

//���� ��ü���� �����ϴ� ���̴� ��ü�̴�. 
class CObjectsShader : public CShader
{
protected:
    CGameObject** m_ppObjects{};
    int m_nObjects{};
public:
    CObjectsShader();
    virtual ~CObjectsShader();

    virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void AnimateObjects(float fTimeElapsed);
    virtual void ReleaseObjects();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob**ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
    virtual void CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);

    virtual void ReleaseUploadBuffers();

    virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);
};