#include "stdafx.h"
#include "Scene.h"
#include "GameTimer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Player.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�׷��� ��Ʈ �ñ״��ĸ� �����Ѵ�. 
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//������Ʈ ���̴��� �����Ѵ�.
	m_nShaders = 1;
	m_pShaders = new CObjectsShader[m_nShaders]{};
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
	//��ƼŬ ���̴��� �����Ѵ�.
	NumOfParticles = m_pShaders->GetNumOfObjects();
	ParticlesShader = new CParticlesShader[NumOfParticles]{};
	for (int i = 0; i < NumOfParticles; ++i)
	{
		ParticlesShader[i].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		ParticlesShader[i].BuildObjects(pd3dDevice, pd3dCommandList);
	}
	//Ÿ�� ���̴��� �����Ѵ�.
	TilesShader = new CTilesShader{};
	TilesShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	TilesShader->BuildObjects(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;

	for (int i = 0; i < NumOfParticles; ++i)
	{
		ParticlesShader[i].ReleaseShaderVariables();
		ParticlesShader[i].ReleaseObjects();
	}
	if (ParticlesShader) delete[] ParticlesShader;

	TilesShader->ReleaseShaderVariables();
	TilesShader->ReleaseObjects();
	if (TilesShader) delete TilesShader;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) 
{ 
	return false; 
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed) 
{ 
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].AnimateObjects(fTimeElapsed);
	for (int i = 0; i < NumOfParticles; ++i) ParticlesShader[i].AnimateObjects(fTimeElapsed);
	TilesShader->AnimateObjects(fTimeElapsed);
	TilesShader->UpdatePosition(Player->GetPosition().z);
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	//��Ʈ �ñ״��ĸ� ���������ο� ����(����)�Ѵ�. 
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//���� �������ϴ� ���� ���� �����ϴ� ���̴� ��ü(���� ��ü�� �����ϴ� ��ü)���� �������ϴ� ���̴�. 
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].Render(pd3dCommandList, pCamera);
	for (int i = 0; i < NumOfParticles; ++i) ParticlesShader[i].Render(pd3dCommandList, pCamera);
	TilesShader->Render(pd3dCommandList, pCamera);
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();
	for (int i = 0; i < NumOfParticles; ++i) ParticlesShader[i].ReleaseUploadBuffers();
	TilesShader->ReleaseUploadBuffers();
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return m_pd3dGraphicsRootSignature;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature{};

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;		//b0: World
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;		//b1: View, Projection
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0;	//t0: SRV
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc{};
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob{};
	ID3DBlob* pd3dErrorBlob{};
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return pd3dGraphicsRootSignature;
}

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	if (!pCamera) return nullptr;

	XMFLOAT4X4 xmf4x4View{ pCamera->GetViewMatrix() };
	XMFLOAT4X4 xmf4x4Projection{ pCamera->GetProjectionMatrix() };
	D3D12_VIEWPORT d3dViewport{ pCamera->GetViewport() };
	XMFLOAT3 xmf3PickPosition{};

	/*ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�. �� ����� ī�޶�
	��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;

	int nIntersected{};
	float fHitDistance{ FLT_MAX }, fNearestHitDistance{ FLT_MAX };
	CGameObject* pIntersectedObject{}, * pNearestObject{};

	//���̴��� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ͽ� ī�޶�� ���� ����� ���� ��ü�� ���Ѵ�. 
	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_pShaders[i].PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if (pIntersectedObject && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = pIntersectedObject;
		}
	}

	return pNearestObject;
}

void CScene::SetPlayer(CPlayer* NewPlayer)
{
	//�÷��̾ �����Ѵ�.
	Player = NewPlayer;
	//�÷��̾��� �Ҹ����̴��� �����Ѵ�.
	if (Player) BulletsShader = Player->GetBulletShader();
}

bool CScene::IsCollided(const BoundingOrientedBox& Lhs, const BoundingOrientedBox& Rhs)
{
	return Lhs.Contains(Rhs) != ContainmentType::DISJOINT;
}

void CScene::ProcessCollision()
{
	if (!m_pShaders || !BulletsShader) return;

	static const auto Objects{ m_pShaders->GetObjects() };
	static const auto& Bullets{ BulletsShader->GetBullets() };

	// ť�� & �Ѿ� ó��
	for (int i = 0; i < m_pShaders->GetNumOfObjects(); ++i)
	{
		for (int j = 0; j < BulletsShader->GetNumOfBullets(); ++j)
		{
			BoundingOrientedBox LeftBox{ Objects[i]->GetBoundingBox() };
			BoundingOrientedBox RightBox{ Bullets[j]->GetBoundingBox() };

			LeftBox.Transform(LeftBox, XMLoadFloat4x4(&Objects[i]->GetWorldMatrix()));
			RightBox.Transform(RightBox, XMLoadFloat4x4(&Bullets[j]->GetWorldMatrix()));

			if (IsCollided(LeftBox, RightBox) && Objects[i]->GetVisible())
			{
				BulletsShader->ReleaseBullet(j);
				Objects[i]->SetVisible(false);

				//��ƼŬ ����
				ParticlesShader[i].ExecuteParticle(Objects[i]->GetPosition());
				//��ƼŬ ����
				AddTimerQueue([=]() 
					{
						Objects[i]->SetVisible(true);
						ParticlesShader[i].StopParticle();
					}, 1500);
			}
		}
	}
}
