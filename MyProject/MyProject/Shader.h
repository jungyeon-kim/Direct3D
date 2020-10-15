#pragma once

class CGameObject;

// ���̴� �ҽ� �ڵ带 �������ϰ� �׷��Ƚ� ���� ��ü�� ����
class CShader
{
private:
    int RefCount{};
protected:
    // ���̴��� �����ϴ� ���� ��ü��
    CGameObject** Objects{};
    int NumOfObject{};

    // ���������� ���� ��ü��
    ID3D12PipelineState** PipelineStates{};
    int NumOfPipelineState{};
public:
    CShader();
    virtual ~CShader();

    void AddRef() { ++RefCount; }
    void Release() { if (--RefCount <= 0) delete this; }

    virtual D3D12_INPUT_LAYOUT_DESC SetInputLayout();
    virtual D3D12_RASTERIZER_DESC SetRasterizerState();
    virtual D3D12_BLEND_DESC SetBlendState();
    virtual D3D12_DEPTH_STENCIL_DESC SetDepthStencilState();

    virtual D3D12_SHADER_BYTECODE SetVertexShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob);
    virtual D3D12_SHADER_BYTECODE SetPixelShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob);

    D3D12_SHADER_BYTECODE CompileShaderFromFile(LPCWSTR FileName, LPCSTR ShaderName,
        LPCSTR ShaderProfile, ID3DBlob** ShaderBlob);

    virtual void CreatePipelineState(ID3D12Device* Device, ID3D12RootSignature* RootSignature);

    virtual void CreateShaderVariables(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList) {}
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* CommandList) {}
    virtual void ReleaseShaderVariables() {}

    virtual void ReleaseUploadBuffers();

    virtual void AddObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* Context = NULL);
    virtual void Animate(float ElapsedTime);
    virtual void ReleaseObjects();

    virtual void OnPrepareRender(ID3D12GraphicsCommandList* CommandList);
    virtual void Render(ID3D12GraphicsCommandList* CommandList);
};