#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

void CScene::BuildObjects() 
{
	m_ppObjects = new CGameObject*[nObjects];
	objectBox = new BoundingBox[nObjects];
	particles = new std::vector<CGameObject*>[nObjects];
	tileBox = { XMFLOAT4{0.0f, -1.0f, 0.0f, 50.0f},XMFLOAT4{0.0f, 1.0f, 0.0f, 50.0f},
		XMFLOAT4{1.0f, 0.0f, 0.0f, 50.0f},XMFLOAT4{-1.0f, 0.0f, 0.0f, 50.0f} };

	// 큐브 생성
	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	m_ppObjects[0] = new CGameObject(); 
	m_ppObjects[0]->SetMesh(pCubeMesh); 
	m_ppObjects[0]->SetColor(RGB(255, 0, 0)); 
	m_ppObjects[0]->SetPosition(-14.0f, 0.0f, +10.0f);
	m_ppObjects[0]->SetRotationAxis(XMFLOAT3(1.0f, 1.0f, 0.0f)); 
	m_ppObjects[0]->SetRotationSpeed(90.0f); 
	m_ppObjects[0]->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f)); 
	m_ppObjects[0]->SetMovingSpeed(5.0f);

	m_ppObjects[1] = new CGameObject(); 
	m_ppObjects[1]->SetMesh(pCubeMesh); 
	m_ppObjects[1]->SetColor(RGB(0, 0, 255)); 
	m_ppObjects[1]->SetPosition(+13.5f, 0.0f, +14.0f); 
	m_ppObjects[1]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f)); 
	m_ppObjects[1]->SetRotationSpeed(180.0f); 
	m_ppObjects[1]->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f)); 
	m_ppObjects[1]->SetMovingSpeed(5.0f);

	m_ppObjects[2] = new CGameObject(); 
	m_ppObjects[2]->SetMesh(pCubeMesh);
	m_ppObjects[2]->SetColor(RGB(0, 255, 0)); 
	m_ppObjects[2]->SetPosition(0.0f, +5.0f, 20.0f); 
	m_ppObjects[2]->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 1.0f)); 
	m_ppObjects[2]->SetRotationSpeed(30.15f); 
	m_ppObjects[2]->SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f)); 
	m_ppObjects[2]->SetMovingSpeed(5.5f);

	m_ppObjects[3] = new CGameObject(); 
	m_ppObjects[3]->SetMesh(pCubeMesh); 
	m_ppObjects[3]->SetColor(RGB(0, 255, 255)); 
	m_ppObjects[3]->SetPosition(0.0f, 0.0f, 40.0f); 
	m_ppObjects[3]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f)); 
	m_ppObjects[3]->SetRotationSpeed(40.6f); 
	m_ppObjects[3]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f)); 
	m_ppObjects[3]->SetMovingSpeed(3.5f);
	
	m_ppObjects[4] = new CGameObject(); 
	m_ppObjects[4]->SetMesh(pCubeMesh); 
	m_ppObjects[4]->SetColor(RGB(128, 0, 255)); 
	m_ppObjects[4]->SetPosition(10.0f, 10.0f, 50.0f); 
	m_ppObjects[4]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f)); 
	m_ppObjects[4]->SetRotationSpeed(50.06f); 
	m_ppObjects[4]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f)); 
	m_ppObjects[4]->SetMovingSpeed(15.0f);

	// 큐브의 바운딩 박스 생성
	static XMVECTOR min = XMVectorSet(-2.0f, -2.0f, -2.0f, 1.0f);
	static XMVECTOR max = XMVectorSet(+2.0f, +2.0f, +2.0f, 1.0f);

	for (int i = 0; i < nObjects; ++i) BoundingBox::CreateFromPoints(objectBox[i], min, max);

	// 맵 생성
	CMapMesh* pMapMesh = new CMapMesh{ 100.0f, 100.0f, 10.0f };

	for (int i = 0; i < numOfTile; ++i)
	{
		mapTiles[i] = new CGameObject{};
		mapTiles[i]->SetMesh(pMapMesh);
		mapTiles[i]->SetColor(RGB(0, 0, 0));
		mapTiles[i]->SetPosition(0.0f, 0.0f, 
			static_cast<float>(i * 10) + m_pPlayer->m_xmf3Position.z - numOfTile * 5);
	}
}

void CScene::ReleaseObjects() 
{
	for (int i = 0; i < nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;
}

void CScene::Animate(float fElapsedTime) 
{ 
	for (int i = 0; i < nObjects; i++)
	{
		m_ppObjects[i]->Animate(fElapsedTime);
		for (int j = 0; j < particles[i].size(); ++j) particles[i][j]->Animate(fElapsedTime);
	}

	if (abs(mapTiles[numOfTile / 2]->m_xmf4x4World._43 - m_pPlayer->m_xmf3Position.z) > 10.0f)
		for (int i = 0; i < numOfTile; ++i)
			mapTiles[i]->SetPosition(0.0f, 0.0f,
				static_cast<float>(i * 10) + m_pPlayer->m_xmf3Position.z - numOfTile * 5);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) 
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewProjectTransform(&pCamera->m_xmf4x4ViewProject);
	
	for (int i = 0; i < nObjects; i++)
	{
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);
		for (int j = 0; j < particles[i].size(); ++j) particles[i][j]->Render(hDCFrameBuffer, pCamera);
	}
	for (int i = 0; i < numOfTile; i++) mapTiles[i]->Render(hDCFrameBuffer, pCamera);
}

bool CScene::IsCollided(const BoundingBox& left, const BoundingBox& right)
{
	return left.Contains(right) != ContainmentType::DISJOINT;
}

void CScene::ProcessCollision()
{
	if (m_pPlayer)
	{
		// 큐브와 총알 체크
		for (int i = 0; i < nObjects; ++i)
		{
			for (int j = 0; j < m_pPlayer->GetNumOfBullet(); ++j)
			{
				BoundingBox leftBox{ objectBox[i] };
				BoundingBox rightBox{ m_pPlayer->GetBulletBox(j) };

				leftBox.Transform(leftBox, XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World));
				rightBox.Transform(rightBox, XMLoadFloat4x4(&m_pPlayer->GetBullet(j)->m_xmf4x4World));

				if (IsCollided(leftBox, rightBox) && m_ppObjects[i]->m_bActive)
				{
					m_ppObjects[i]->SetMesh(new CMesh{});
					m_ppObjects[i]->m_bActive = false;
					m_pPlayer->ReleaseBullet(j);

					// 파티클 생성
					if (!particles[i].size())
						for (int k = 0; k < 200; ++k)
						{
							static CCubeMesh* pCubeMesh = new CCubeMesh{ 0.5f, 0.5f, 0.5f };

							particles[i].emplace_back();
							particles[i].back() = new CGameObject();
							particles[i].back()->SetMesh(pCubeMesh);
							particles[i].back()->SetColor(m_ppObjects[i]->m_dwColor);
							particles[i].back()->SetPosition(m_ppObjects[i]->m_xmf4x4World._41, m_ppObjects[i]->m_xmf4x4World._42, m_ppObjects[i]->m_xmf4x4World._43);
							particles[i].back()->SetMovingDirection(XMFLOAT3{
								float(rand() % 10000 - 5000) / 10000.0f, float(rand() % 10000 - 5000) / 10000.0f, float(rand() % 10000 - 5000) / 10000.0f });
							particles[i].back()->SetMovingSpeed(20.0f);
						}
				}
			}
			// 파티클 제거
			static int timer[nObjects]{};

			if (timer[i] > 100)
			{
				m_ppObjects[i]->SetMesh(new CCubeMesh{ 4.0f, 4.0f, 4.0f });
				m_ppObjects[i]->m_bActive = true;
				m_ppObjects[i]->isPicked = false;
				particles[i].clear();
				timer[i] = 0;
			}
			if (particles[i].size() > 0) ++timer[i];
		}

		// 큐브와 큐브 체크
		for (int i = 0; i < nObjects; ++i)
			for (int j = i; j < nObjects; ++j)
			{
				BoundingBox leftBox{ objectBox[i] };
				BoundingBox rightBox{ objectBox[j] };

				leftBox.Transform(leftBox, XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World));
				rightBox.Transform(rightBox, XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World));

				if (IsCollided(leftBox, rightBox))
				{
					XMFLOAT3& leftObjMovDir{ m_ppObjects[i]->m_xmf3MovingDirection };
					XMFLOAT3& rightObjMovDir{ m_ppObjects[j]->m_xmf3MovingDirection };
					
					leftObjMovDir = XMFLOAT3{ -leftObjMovDir.x, -leftObjMovDir.y , -leftObjMovDir.y };
					rightObjMovDir = XMFLOAT3{ -rightObjMovDir.x, -rightObjMovDir.y , -rightObjMovDir.y };
				}
			}

		// 플레이어와 맵 체크
		BoundingBox box{ m_pPlayer->GetPlayerBox() };
		box.Transform(box, 1.0f, FXMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f },
			FXMVECTOR{ m_pPlayer->m_xmf3Position.x, m_pPlayer->m_xmf3Position.y, m_pPlayer->m_xmf3Position.z, 1.0f });

		for (int i = 0; i < 4; ++i)
			if (box.Intersects(XMLoadFloat4(&tileBox[i])))
			{
				m_pPlayer->m_xmf3Position = XMFLOAT3{ 0.0f, 0.0f, m_pPlayer->m_xmf3Position.z };
				m_pPlayer->GetCamera()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, m_pPlayer->m_xmf3Position.z });
			}

		// 큐브와 맵 체크
		for (int i = 0; i < nObjects; ++i)
			for (int j = 0; j < 4; ++j)
			{
				BoundingBox box{ objectBox[i] };

				box.Transform(box, XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World));

				if (box.Intersects(XMLoadFloat4(&tileBox[j])))
				{
					XMFLOAT3& objMovDir{ m_ppObjects[i]->m_xmf3MovingDirection };
					objMovDir = XMFLOAT3{ -objMovDir.x, -objMovDir.y , -objMovDir.y };
				}
			}
	}
}

int CScene::CheckPicking(XMFLOAT3& rayOrigin, XMFLOAT3& rayDir)
{
	int cnt{ -1 };

	for (int i = 0; i < nObjects; ++i) 
	{
		float dist{};
		BoundingBox box{ objectBox[i] };

		box.Transform(box, XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World));

		// 해당 큐브가 피킹되지않았고 rayOrigin과 rayDir을 가지는 직선과 교차한다면
		if (!m_ppObjects[i]->isPicked &&
			box.Intersects(XMLoadFloat3(&rayOrigin), XMVector4Normalize(XMLoadFloat3(&rayDir)), dist))
		{
			// 광선과 교차하는 큐브가 2개이상일 때
			if (cnt != -1) 
			{
				float lastObjZ{ m_ppObjects[cnt]->m_xmf4x4World._43 };
				float currObjZ{ m_ppObjects[i]->m_xmf4x4World._43 };
				cnt = lastObjZ - rayOrigin.z > currObjZ - rayOrigin.z ? i : cnt;
				continue;
			}
			cnt = i;
			m_ppObjects[cnt]->isPicked = true;
		}
	}
	return cnt;
}