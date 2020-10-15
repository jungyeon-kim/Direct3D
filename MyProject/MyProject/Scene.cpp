#include "stdafx.h"
#include "Scene.h"
#include "GameTimer.h"
#include "Shader.h"

void CScene::AddObjects(ID3D12Device* Device, ID3D12GraphicsCommandList* CommandList)
{
	// 루트 시그니쳐를 생성 
	RootSignature = CreateRootSignature(Device);

	// 셰이더 생성
	NumOfShader = 1;
	Shaders = new CShader*[NumOfShader];
	CShader* Shader{ new CShader() };
	Shader->CreatePipelineState(Device, RootSignature);
	Shader->AddObjects(Device, CommandList, NULL);
	Shaders[0] = Shader;
}

void CScene::ReleaseObjects()
{
	if (RootSignature) RootSignature->Release();
	if (Shaders)
	{
		for (int i = 0; i < NumOfShader; ++i)
		{
			Shaders[i]->ReleaseShaderVariables();
			Shaders[i]->ReleaseObjects();
			Shaders[i]->Release();
		}
		delete[] Shaders;
	}
}

bool CScene::ProcessMouseMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::ProcessKeyboardMsg(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::ProcessInput() 
{
	return false;
}

void CScene::Animate(float ElapsedTime) 
{ 
	for (int i = 0; i < NumOfShader; ++i) Shaders[i]->Animate(ElapsedTime);
}

void CScene::Render(ID3D12GraphicsCommandList* CommandList)
{
	// 루트 시그니쳐를 파이프라인에 Set
	CommandList->SetGraphicsRootSignature(RootSignature);
	// 씬을 렌더링하는 것은 씬을 구성하는 셰이더(셰이더가 포함하는 객체)들을 렌더링하는 것
	for (int i = 0; i < NumOfShader; ++i) Shaders[i]->Render(CommandList);
}

void CScene::ReleaseUploadBuffers()
{
	if (Shaders)
		for (int i = 0; i < NumOfShader; ++i)
			if (Shaders[i])
				Shaders[i]->ReleaseUploadBuffers();
}

ID3D12RootSignature* CScene::GetRootSignature()
{
	return RootSignature;
}

ID3D12RootSignature* CScene::CreateRootSignature(ID3D12Device* Device)
{
	ID3D12RootSignature* RootSignature{};

	// 루트 파라미터가 없는(빈) 루트 시그너쳐 생성
	D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc{};
	RootSignatureDesc.NumParameters = 0;
	RootSignatureDesc.pParameters = NULL;
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = NULL;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* SignatureBlob{};
	ID3DBlob* ErrorBlob{};
	D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SignatureBlob, &ErrorBlob);
	Device->CreateRootSignature(0, SignatureBlob->GetBufferPointer(),
		SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&RootSignature);
	if (SignatureBlob) SignatureBlob->Release();
	if (ErrorBlob) ErrorBlob->Release();

	return RootSignature;
}