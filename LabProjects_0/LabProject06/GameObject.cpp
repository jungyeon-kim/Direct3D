#include "stdafx.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��. 
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPrepareRender();
	//���� ��ü�� ���̴� ��ü�� ����Ǿ� ������ ���̴� ���� ��ü�� �����Ѵ�. 
	if (m_pShader) m_pShader->Render(pd3dCommandList);
	//���� ��ü�� �޽��� ����Ǿ� ������ �޽��� �������Ѵ�. 
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);
}