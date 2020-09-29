#pragma once

class CGameObject;

//���̴� �ҽ� �ڵ带 �������ϰ� �׷��Ƚ� ���� ��ü�� �����Ѵ�. 
class CShader
{
private:
    int m_nReferences{};
protected:
    //���̴��� �����ϴ� ���� ��ü���� ����Ʈ(�迭)�̴�. 
    CGameObject** m_ppObjects{};
    int m_nObjects{};
    //���������� ���� ��ü���� ����Ʈ(�迭)�̴�. 
    ID3D12PipelineState** m_ppd3dPipelineStates{};
    int m_nPipelineStates{};
public:
    CShader();
    virtual ~CShader();

    void AddRef() { ++m_nReferences; }
    void Release() { if (--m_nReferences <= 0) delete this; }

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
    virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
    virtual D3D12_BLEND_DESC CreateBlendState();
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

    D3D12_SHADER_BYTECODE CompileShaderFromFile(LPCWSTR pszFileName, LPCSTR pszShaderName,
        LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

    virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature);

    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {}
    virtual void ReleaseShaderVariables() {}

    virtual void ReleaseUploadBuffers();

    virtual void AddObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
    virtual void Animate(float fTimeElapsed);
    virtual void ReleaseObjects();

    virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};