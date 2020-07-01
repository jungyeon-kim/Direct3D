#pragma once

class CGameObject;
class CCamera;

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다. 
struct CB_GAMEOBJECT_INFO
{
    XMFLOAT4X4 m_xmf4x4World;
};

//인스턴스 정보(게임 객체의 월드 변환 행렬과 객체의 색상)를 위한 구조체이다. 
struct VS_VB_INSTANCE
{
    XMFLOAT4X4 m_xmf4x4Transform;
    XMFLOAT4 m_xmcColor;
};

//셰이더 소스 코드를 컴파일하고 그래픽스 상태 객체를 생성한다. 
class CShader
{
private:
    int m_nReferences{};
protected:
    //파이프라인 상태 객체들의 리스트(배열)이다. 
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

//게임 객체들을 포함하는 셰이더 객체이다. 
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

class CInstancingShader : public CObjectsShader
{
protected:
    //인스턴스 정점 버퍼와 정점 버퍼 뷰이다. 
    ID3D12Resource* m_pd3dcbGameObjects{};
    VS_VB_INSTANCE* m_pcbMappedGameObjects{};
    D3D12_VERTEX_BUFFER_VIEW m_d3dInstancingBufferView{};
public:
    CInstancingShader();
    virtual ~CInstancingShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature);
    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseObjects();

    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
};