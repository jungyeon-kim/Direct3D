#include "stdafx.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
	if (Mesh) Mesh->Release();
	if (Shader)
	{
		Shader->ReleaseShaderVariables();
		Shader->Release();
	}
}

void CGameObject::SetShader(CShader* NewShader)
{
	if (Shader) Shader->Release();
	Shader = NewShader;
	if (Shader) Shader->AddRef();
}

void CGameObject::SetMesh(CMesh* NewMesh)
{
	if (Mesh) Mesh->Release();
	Mesh = NewMesh;
	if (Mesh) Mesh->AddRef();
}

void CGameObject::ReleaseUploadBuffers()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸
	if (Mesh) Mesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* CommandList)
{
	OnPrepareRender();

	// 게임 객체에 메쉬가 연결되어 있으면 메쉬를 렌더링
	if (Mesh) Mesh->Render(CommandList);
}