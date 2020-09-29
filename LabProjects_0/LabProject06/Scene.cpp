#include "stdafx.h"
#include "Scene.h"
#include "GameTimer.h"
#include "Shader.h"

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//���� �׸��� ���� ���̴� ��ü�� �����Ѵ�. 
	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];
	CShader* pShader{ new CShader() };
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);
	m_ppShaders[0] = pShader;
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::ProcessInput() 
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed) 
{ 
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)�Ѵ�. 
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	//���� �������ϴ� ���� ���� �����ϴ� ���̴�(���̴��� �����ϴ� ��ü)���� �������ϴ� ���̴�. 
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dCommandList);
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++) if (m_ppShaders[j])
			m_ppShaders[j]->ReleaseUploadBuffers();
	}
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature{};

	//�Ű������� ����(��) ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = NULL;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pd3dSignatureBlob{};
	ID3DBlob* pd3dErrorBlob{};
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return pd3dGraphicsRootSignature;
}