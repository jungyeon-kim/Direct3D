#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"
#include "Mesh.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	if (PipelineStates)
	{
		for (int i = 0; i < NumOfPipelineState; ++i)
			if (PipelineStates[i])
				PipelineStates[i]->Release();
		delete[] PipelineStates;
	}
}

// �����Ͷ����� ����
D3D12_RASTERIZER_DESC CShader::SetRasterizerState()
{
	D3D12_RASTERIZER_DESC RasterizerDesc{};
	RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	RasterizerDesc.FrontCounterClockwise = FALSE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;
	RasterizerDesc.ForcedSampleCount = 0;
	RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return RasterizerDesc;
}

// ����-���ٽ� ����
D3D12_DEPTH_STENCIL_DESC CShader::SetDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC DepthStencilDesc{};
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	DepthStencilDesc.StencilEnable = FALSE;
	DepthStencilDesc.StencilReadMask = 0x00;
	DepthStencilDesc.StencilWriteMask = 0x00;
	DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return DepthStencilDesc;
}

// ���� ����
D3D12_BLEND_DESC CShader::SetBlendState()
{
	D3D12_BLEND_DESC BlendDesc{};
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;
	BlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return BlendDesc;
}

// ��ǲ ���̾ƿ� ����
D3D12_INPUT_LAYOUT_DESC CShader::SetInputLayout()
{
	UINT NumOfInputElementDesc{ 2 };
	D3D12_INPUT_ELEMENT_DESC* InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[NumOfInputElementDesc] };

	// ������ ��ġ ����(POSITION)�� ����(COLOR)�� ����
	InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc{};
	InputLayoutDesc.pInputElementDescs = InputElementDescs;
	InputLayoutDesc.NumElements = NumOfInputElementDesc;

	return InputLayoutDesc;
}

// ���� ���̴� ����
D3D12_SHADER_BYTECODE CShader::SetVertexShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", ShaderName, "vs_5_1", ShaderBlob);
}

// �ȼ� ���̴� ����
D3D12_SHADER_BYTECODE CShader::SetPixelShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", ShaderName, "ps_5_1", ShaderBlob);
}

// ���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(LPCWSTR FileName, LPCSTR ShaderName, LPCSTR ShaderProfile, ID3DBlob** ShaderBlob)
{
	D3DCompileFromFile(FileName, NULL, NULL, ShaderName, ShaderProfile, 0, 0, ShaderBlob, NULL);

	D3D12_SHADER_BYTECODE ShaderByteCode{};
	ShaderByteCode.BytecodeLength = (*ShaderBlob)->GetBufferSize();
	ShaderByteCode.pShaderBytecode = (*ShaderBlob)->GetBufferPointer();

	return ShaderByteCode;
}

// �׷��Ƚ� ���������� ���� ��ü�� ���� �� ����
void CShader::CreatePipelineState(ID3D12Device* Device, ID3D12RootSignature* RootSignature)
{
	//�׷��Ƚ� ���������� ���� ��ü �迭�� �����Ѵ�. 
	NumOfPipelineState = 1;
	PipelineStates = new ID3D12PipelineState*[NumOfPipelineState];

	ID3DBlob* VertexShaderBlob{}, *PixelShaderBlob{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc{};
	PipelineStateDesc.pRootSignature = RootSignature;
	PipelineStateDesc.InputLayout = SetInputLayout();
	PipelineStateDesc.VS = SetVertexShader("VSMain", &VertexShaderBlob);
	PipelineStateDesc.PS = SetPixelShader("PSMain", &PixelShaderBlob);
	PipelineStateDesc.RasterizerState = SetRasterizerState();
	PipelineStateDesc.BlendState = SetBlendState();
	PipelineStateDesc.DepthStencilState = SetDepthStencilState();
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	Device->CreateGraphicsPipelineState(&PipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&PipelineStates[0]);

	if (VertexShaderBlob) VertexShaderBlob->Release();
	if (PixelShaderBlob) PixelShaderBlob->Release();

	if (PipelineStateDesc.InputLayout.pInputElementDescs) delete[] PipelineStateDesc.InputLayout.pInputElementDescs;
}

// ���̴� ��ü�� �����ϴ� ���� ��ü���� ����
void CShader::AddObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList, void* Context)
{
	CTriangleMesh* TriangleMesh{ new CTriangleMesh(Device, CommandList) };

	NumOfObject = 1;
	Objects = new CGameObject*[NumOfObject];

	Objects[0] = new CGameObject();
	Objects[0]->SetMesh(TriangleMesh);
}

void CShader::ReleaseObjects()
{
	if (Objects)
	{
		for (int i = 0; i < NumOfObject; ++i) if (Objects[i]) delete Objects[i];
		delete[] Objects;
	}
}

void CShader::Animate(float fTimeElapsed)
{
	for (int i = 0; i < NumOfObject; ++i) Objects[i]->Animate(fTimeElapsed);
}

void CShader::ReleaseUploadBuffers()
{
	if (Objects)
		for (int i = 0; i < NumOfObject; ++i)
			if (Objects[i])
				Objects[i]->ReleaseUploadBuffers();
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���������ο� �׷��Ƚ� ���� ��ü ����
	pd3dCommandList->SetPipelineState(PipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

	for (int i = 0; i < NumOfObject; ++i)
		if (Objects[i]) Objects[i]->Render(pd3dCommandList);
}