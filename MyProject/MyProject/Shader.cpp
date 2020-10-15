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

// 래스터라이저 설정
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

// 깊이-스텐실 설정
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

// 블렌딩 설정
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

// 인풋 레이아웃 설정
D3D12_INPUT_LAYOUT_DESC CShader::SetInputLayout()
{
	UINT NumOfInputElementDesc{ 2 };
	D3D12_INPUT_ELEMENT_DESC* InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[NumOfInputElementDesc] };

	// 정점은 위치 벡터(POSITION)와 색상(COLOR)을 가짐
	InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	InputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc{};
	InputLayoutDesc.pInputElementDescs = InputElementDescs;
	InputLayoutDesc.NumElements = NumOfInputElementDesc;

	return InputLayoutDesc;
}

// 정점 셰이더 설정
D3D12_SHADER_BYTECODE CShader::SetVertexShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", ShaderName, "vs_5_1", ShaderBlob);
}

// 픽셀 셰이더 설정
D3D12_SHADER_BYTECODE CShader::SetPixelShader(LPCSTR ShaderName, ID3DBlob** ShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", ShaderName, "ps_5_1", ShaderBlob);
}

// 셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(LPCWSTR FileName, LPCSTR ShaderName, LPCSTR ShaderProfile, ID3DBlob** ShaderBlob)
{
	D3DCompileFromFile(FileName, NULL, NULL, ShaderName, ShaderProfile, 0, 0, ShaderBlob, NULL);

	D3D12_SHADER_BYTECODE ShaderByteCode{};
	ShaderByteCode.BytecodeLength = (*ShaderBlob)->GetBufferSize();
	ShaderByteCode.pShaderBytecode = (*ShaderBlob)->GetBufferPointer();

	return ShaderByteCode;
}

// 그래픽스 파이프라인 상태 객체를 생성 및 설정
void CShader::CreatePipelineState(ID3D12Device* Device, ID3D12RootSignature* RootSignature)
{
	//그래픽스 파이프라인 상태 객체 배열을 생성한다. 
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

// 셰이더 객체가 포함하는 게임 객체들을 생성
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
	// 파이프라인에 그래픽스 상태 객체 설정
	pd3dCommandList->SetPipelineState(PipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList);

	for (int i = 0; i < NumOfObject; ++i)
		if (Objects[i]) Objects[i]->Render(pd3dCommandList);
}